import requests
from bs4 import BeautifulSoup
import re
import time
import json
from collections import deque

# 디버그 모드 설정
DEBUG = True

# HTTP 요청 시 사용할 헤더
HEADERS = {
    'Accept-Language': 'en-US,en;q=0.5',
    'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64)'
}


def get_actor_url(actor_name):
    """
    IMDb 검색 URL을 크롤링하여 배우 페이지 URL을 반환합니다.
    검색 실패 시 None을 반환합니다.
    """
    query = actor_name.strip().replace(' ', '+')
    search_url = f'https://www.imdb.com/find?q={query}&s=nm'
    if DEBUG:
        print(f"[DEBUG] Searching actor URL: {search_url}")
    resp = requests.get(search_url, headers=HEADERS)
    soup = BeautifulSoup(resp.text, 'html.parser')
    # 1) 테이블 기반 검색 결과
    link = soup.select_one('table.findList tr.findResult td.result_text a[href^="/name/"]')
    if link:
        url = 'https://www.imdb.com' + link['href'].split('?')[0]
        if DEBUG:
            print(f"[DEBUG] Found actor URL (table): {url}")
        return url
    # 2) 페이지 내 첫 번째 /name/nm 링크
    link = soup.find('a', href=re.compile(r'^/name/nm'))
    if link:
        url = 'https://www.imdb.com' + link['href'].split('?')[0]
        if DEBUG:
            print(f"[DEBUG] Found actor URL (fallback): {url}")
        return url
    if DEBUG:
        print(f"[DEBUG] Actor URL not found for: {actor_name}")
    return None


def get_movies_for_actor(actor_url):
    """
    배우의 출연작 URL 리스트를 반환합니다.
    1) 페이지 내 JSON tconst 추출
    2) filmotype 페이지 크롤링
    3) 모바일 filmography 페이지 크롤링
    """
    if DEBUG:
        print(f"[DEBUG] get_movies_for_actor called for: {actor_url}")
    movies = []
    html = ''
    # 1. 페이지 HTML에서 tconst 추출
    try:
        resp = requests.get(actor_url, headers=HEADERS)
        resp.raise_for_status()
        html = resp.text
        tconsts = set(re.findall(r'"tconst":"(tt\d+)"', html))
        for tid in tconsts:
            movies.append(f'https://www.imdb.com/title/{tid}/')
        if DEBUG:
            print(f"[DEBUG] Extracted {len(tconsts)} titles from JSON regex")
    except Exception as e:
        if DEBUG:
            print(f"[DEBUG] JSON regex parsing failed: {e}")
    # 2. filmotype 페이지 크롤링
    if not movies:
        filmotype_url = actor_url.rstrip('/') + '/filmotype/actor/'
        if DEBUG:
            print(f"[DEBUG] Filmotype page URL: {filmotype_url}")
        try:
            resp2 = requests.get(filmotype_url, headers=HEADERS)
            resp2.raise_for_status()
            soup2 = BeautifulSoup(resp2.text, 'html.parser')
            links2 = soup2.find_all('a', href=re.compile(r'^/title/tt\d+'))
            if DEBUG:
                print(f"[DEBUG] Filmotype raw count: {len(links2)}")
            for a in links2:
                href = a['href']
                movies.append('https://www.imdb.com' + href.split('?')[0])
            if DEBUG:
                print(f"[DEBUG] Filmotype page titles: {len(links2)}")
        except Exception as e:
            if DEBUG:
                print(f"[DEBUG] Filmotype fallback failed: {e}")
    # 3. 모바일 filmography 페이지 크롤링
    if not movies:
        actor_id = actor_url.rstrip('/').split('/')[-1]
        mobile_url = f'https://m.imdb.com/name/{actor_id}/filmography'
        if DEBUG:
            print(f"[DEBUG] Mobile filmography URL: {mobile_url}")
        try:
            resp3 = requests.get(mobile_url, headers=HEADERS)
            resp3.raise_for_status()
            soup3 = BeautifulSoup(resp3.text, 'html.parser')
            links3 = soup3.find_all('a', href=re.compile(r'^/title/tt\d+'))
            if DEBUG:
                print(f"[DEBUG] Mobile raw count: {len(links3)}")
            for a in links3:
                href = a['href']
                movies.append('https://www.imdb.com' + href.split('?')[0])
            if DEBUG:
                print(f"[DEBUG] Mobile page titles: {len(links3)}")
        except Exception as e:
            if DEBUG:
                print(f"[DEBUG] Mobile fallback failed: {e}")
        # 4. Actor page filmo-row fallback
    if not movies and html:
        if DEBUG:
            print("[DEBUG] Actor page filmo-row fallback")
        try:
            soup_main = BeautifulSoup(html, 'html.parser')
            rows = soup_main.select('div.filmo-row a[href^="/title/tt"]')
            if DEBUG:
                print(f"[DEBUG] filmo-row count: {len(rows)}")
            for a in rows:
                href = a['href']
                movies.append('https://www.imdb.com' + href.split('?')[0])
        except Exception as e:
            if DEBUG:
                print(f"[DEBUG] filmo-row fallback failed: {e}")
    # 중복 제거하여 순서 유지
    seen = set()
    unique = []
    for m in movies:
        if m not in seen:
            seen.add(m)
            unique.append(m)
    if DEBUG:
        print(f"[DEBUG] Final movie count: {len(unique)}; sample: {unique[:5]}")
    return unique


def get_cast_for_movie(movie_url):
    """
    영화 출연 배우 URL 리스트를 반환합니다.
    1) JSON-LD 파싱
    2) fullcredits 페이지 크롤링(fallback)
    """
    if DEBUG:
        print(f"[DEBUG] get_cast_for_movie called for: {movie_url}")
    cast = []
    # 1. JSON-LD 파싱
    try:
        resp = requests.get(movie_url, headers=HEADERS)
        soup = BeautifulSoup(resp.text, 'html.parser')
        script = soup.find('script', type='application/ld+json')
        if script and script.string:
            data = json.loads(script.string)
            actors = data.get('actor') or data.get('actors') or []
            for actor in actors:
                url = actor.get('url', '')
                if url.startswith('/name/'):
                    full = 'https://www.imdb.com' + url.split('?')[0]
                    cast.append(full)
            if DEBUG:
                print(f"[DEBUG] JSON-LD cast count: {len(cast)}")
            seen = set()
            unique_cast = []
            for c in cast:
                if c not in seen:
                    seen.add(c)
                    unique_cast.append(c)
            return unique_cast
    except Exception as e:
        if DEBUG:
            print(f"[DEBUG] JSON-LD parsing failed: {e}")
    # 2. fullcredits 페이지
    credits_url = movie_url.rstrip('/') + '/fullcredits'
    if DEBUG:
        print(f"[DEBUG] Fallback fullcredits URL: {credits_url}")
    try:
        resp2 = requests.get(credits_url, headers=HEADERS)
        soup2 = BeautifulSoup(resp2.text, 'html.parser')
        rows = soup2.select('table.cast_list tr')[1:]
        for row in rows:
            cell = row.find('td', class_='')
            if cell and cell.a and '/name/' in cell.a['href']:
                url = 'https://www.imdb.com' + cell.a['href'].split('?')[0]
                cast.append(url)
        if DEBUG:
            print(f"[DEBUG] fullcredits cast count: {len(cast)}")
    except Exception as e:
        if DEBUG:
            print(f"[DEBUG] Fullcredits fallback failed: {e}")
    seen2 = set()
    unique_cast2 = []
    for c in cast:
        if c not in seen2:
            seen2.add(c)
            unique_cast2.append(c)
    return unique_cast2


def find_shortest_path(actor1_name, actor2_name):
    a1_url = get_actor_url(actor1_name)
    a2_url = get_actor_url(actor2_name)
    if not a1_url:
        print(f"첫 번째 배우 '{actor1_name}'를 찾을 수 없습니다.")
    if not a2_url:
        print(f"두 번째 배우 '{actor2_name}'를 찾을 수 없습니다.")
    if not a1_url or not a2_url:
        return None
    visited = {a1_url}
    queue = deque([(a1_url, [a1_url])])
    movie_cache = {a1_url: get_movies_for_actor(a1_url)}
    cast_cache = {}
    while queue:
        current_actor, path = queue.popleft()
        if DEBUG:
            print(f"[DEBUG] Visiting: {current_actor}, depth: {len(path)-1}")
        for movie in movie_cache[current_actor]:
            if movie not in cast_cache:
                time.sleep(1)
                cast_cache[movie] = get_cast_for_movie(movie)
            for co_actor in cast_cache[movie]:
                if co_actor == a2_url:
                    return path + [co_actor]
                if co_actor not in visited:
                    visited.add(co_actor)
                    queue.append((co_actor, path + [co_actor]))
    print(f"배우 간 경로를 찾을 수 없습니다: '{actor1_name}' -> '{actor2_name}'")
    return None


def main():
    src = input('첫 번째 배우 이름: ').strip()
    dst = input('두 번째 배우 이름: ').strip()
    path = find_shortest_path(src, dst)
    if path:
        print(f'최단 경로 길이: {len(path)-1}')
        for idx, url in enumerate(path):
            print(f'{idx}: {url}')

if __name__ == '__main__':
    main()
