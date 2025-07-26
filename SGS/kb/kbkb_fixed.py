import requests
import json
from collections import deque
import time
from bs4 import BeautifulSoup

# --- Configuration ---
HEADERS = {
    'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/114.0.0.0 Safari/537.36',
    'Accept-Language': 'en-US,en;q=0.9',
}
DEBUG = False
URL_CACHE = {}

# --- Helper Functions ---
def get_soup(url):
    """Fetches and caches a BeautifulSoup object for a given URL."""
    if url in URL_CACHE:
        return URL_CACHE[url]
    try:
        time.sleep(0.1)
        resp = requests.get(url, headers=HEADERS, timeout=10)
        resp.raise_for_status()
        soup = BeautifulSoup(resp.text, 'html.parser')
        URL_CACHE[url] = soup
        return soup
    except requests.RequestException:
        return None

# --- Core API Functions ---

def get_actor_id(actor_name):
    """Uses IMDb's suggestion API to find the actor's ID."""
    url = f'https://v3.sg.media-imdb.com/suggestion/names/x/{actor_name.strip().lower()}.json'
    if DEBUG: print(f"[API] Getting actor ID for '{actor_name}'")
    try:
        response = requests.get(url, headers=HEADERS, timeout=10)
        response.raise_for_status()
        data = response.json()
        if data.get('d'):
            return data['d'][0]['id']
    except (requests.RequestException, IndexError, KeyError):
        pass
    return None

def get_movies_for_actor(actor_id):
    """Uses a GraphQL query to get an actor's filmography."""
    # This is the correct, current hash for the NameMainFilmography query.
    query_hash = "4c1759da45f770450852c07935494693"
    variables = {"nconst": actor_id, "filmography": {"first": 500}, "total": {"first": 1}}
    url = f"https://caching.graphql.imdb.com/?operationName=NameMainFilmography&variables={json.dumps(variables)}&extensions={json.dumps({'persistedQuery':{'version':1,'sha256Hash':query_hash}})}"
    if DEBUG: print(f"[API] Getting filmography for {actor_id}")
    movies = set()
    try:
        response = requests.get(url, headers=HEADERS, timeout=15)
        response.raise_for_status()
        data = response.json()
        edges = data['data']['name']['filmography']['edges']
        for edge in edges:
            movies.add(edge['node']['media']['id'])
    except (requests.RequestException, KeyError, TypeError):
        pass
    return list(movies)

def get_cast_for_movie(movie_id, limit=50):
    """
    Uses a GraphQL query to get a movie's full cast, with a limit to manage data size.
    """
    query_hash = "963642c5b3ce3b831cd3ce9993630748"
    variables = {"tconst": movie_id, "first": limit} # Use the limit
    url = f"https://caching.graphql.imdb.com/?operationName=FullCredits&variables={json.dumps(variables)}&extensions={json.dumps({'persistedQuery':{'version':1,'sha256Hash':query_hash}})}"
    if DEBUG: print(f"[API] Getting cast for {movie_id} with limit {limit}")
    cast = set()
    try:
        response = requests.get(url, headers=HEADERS, timeout=15)
        response.raise_for_status()
        data = response.json()
        edges = data['data']['title']['credits']['edges']
        for edge in edges:
            cast.add(edge['node']['name']['id'])
    except (requests.RequestException, KeyError, TypeError):
        pass
    return list(cast)

# --- Pathfinding and Main Logic ---

def get_name_for_id(entity_id):
    """Gets the display name for an entity ID by scraping the title tag."""
    type_ = 'name' if entity_id.startswith('nm') else 'title'
    soup = get_soup(f"https://www.imdb.com/{type_}/{entity_id}/")
    if soup and soup.title:
        return soup.title.string.split(' - IMDb')[0].strip()
    return entity_id

def find_shortest_path(start_actor_name, end_actor_name):
    """
    Finds the shortest path using a much faster bi-directional BFS.
    """
    print("배우 ID를 찾는 중...")
    start_id = get_actor_id(start_actor_name)
    end_id = get_actor_id(end_actor_name)

    if not start_id: print(f"오류: 배우 '{start_actor_name}'를 찾을 수 없습니다."); return None
    if not end_id: print(f"오류: 배우 '{end_actor_name}'를 찾을 수 없습니다."); return None
    if start_id == end_id: print("두 배우가 동일합니다."); return [start_id]

    # Setup for bi-directional search
    # Forward search (from start_id)
    q_fwd = deque([(start_id, [start_id])])
    visited_fwd = {start_id: [start_id]}
    # Backward search (from end_id)
    q_bwd = deque([(end_id, [end_id])])
    visited_bwd = {end_id: [end_id]}
    
    cast_cache = {}
    print("경로 탐색 시작 (양방향 탐색)...")

    while q_fwd and q_bwd:
        # --- Forward search step ---
        if len(q_fwd) <= len(q_bwd):
            current_id, path_fwd = q_fwd.popleft()
            movies = get_movies_for_actor(current_id)
            for movie_id in movies:
                cast = cast_cache.get(movie_id)
                if not cast:
                    cast = get_cast_for_movie(movie_id)
                    cast_cache[movie_id] = cast
                
                for actor_id in cast:
                    if actor_id in visited_bwd:
                        # Meeting point found!
                        path_bwd = visited_bwd[actor_id]
                        path_bwd.reverse()
                        return path_fwd + [movie_id] + path_bwd
                    
                    if actor_id not in visited_fwd:
                        new_path = path_fwd + [movie_id, actor_id]
                        visited_fwd[actor_id] = new_path
                        q_fwd.append((actor_id, new_path))
        
        # --- Backward search step ---
        else:
            current_id, path_bwd = q_bwd.popleft()
            movies = get_movies_for_actor(current_id)
            for movie_id in movies:
                cast = cast_cache.get(movie_id)
                if not cast:
                    cast = get_cast_for_movie(movie_id)
                    cast_cache[movie_id] = cast

                for actor_id in cast:
                    if actor_id in visited_fwd:
                        # Meeting point found!
                        path_fwd = visited_fwd[actor_id]
                        path_bwd.reverse()
                        return path_fwd + [movie_id] + path_bwd

                    if actor_id not in visited_bwd:
                        new_path = path_bwd + [movie_id, actor_id]
                        visited_bwd[actor_id] = new_path
                        q_bwd.append((actor_id, new_path))
    
    return None

def main():
    try:
        src = input('첫 번째 배우 이름: ').strip()
        dst = input('두 번째 배우 이름 (기본값: Kevin Bacon): ').strip() or 'Kevin Bacon'
        path_ids = find_shortest_path(src, dst)

        if path_ids:
            bacon_number = (len(path_ids) - 1) // 2
            print("\n" + "="*50)
            print(f"🎉 경로를 찾았습니다! '{src}'의 베이컨 단계는 {bacon_number} 입니다.")
            print("="*50)
            
            print("경로의 이름 정보를 가져오는 중...")
            # Now, fetch the names only for the final path
            path_names = [get_name_for_id(id) for id in path_ids]

            for i in range(bacon_number):
                actor1_name = path_names[i*2]
                movie_name = path_names[i*2 + 1]
                actor2_name = path_names[i*2 + 2]
                
                print(f"\n단계 {i+1}:")
                print(f"  - [{actor1_name}]은(는)")
                print(f"  - 영화 [{movie_name}]에서")
                print(f"  - [{actor2_name}]와(과) 함께 출연했습니다.")
    except KeyboardInterrupt:
        print("\n프로그램을 중단합니다.")
    except Exception as e:
        print(f"\n예상치 못한 오류가 발생했습니다: {e}")

if __name__ == '__main__':
    main()