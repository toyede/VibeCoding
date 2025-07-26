import requests
from bs4 import BeautifulSoup
import pandas as pd
import time
from collections import deque, defaultdict
import re
import json
import os

class MovieBaconGame:
    def __init__(self):
        self.actors_data = pd.DataFrame()
        self.graph = defaultdict(list)
        self.actors_list = []
        self.session = requests.Session()
        self.session.headers.update({
            'User-Agent': 'Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36'
        })
        
    def crawl_top_250(self):
        """IMDB Top 250 영화 목록 크롤링"""
        print("IMDB Top 25 영화 목록을 가져오는 중...")
        
        url = "https://www.imdb.com/chart/top/"
        try:
            response = self.session.get(url)
            response.raise_for_status()
            
            soup = BeautifulSoup(response.content, 'html.parser')
            movies = []
            
            # IMDB Top 250 페이지의 영화 항목들 찾기
            movie_items = soup.find_all('li', class_='ipc-metadata-list-summary-item')
            
            for item in movie_items[:25]:  # 상위 25개만
                try:
                    # 영화 제목 추출
                    title_elem = item.find('h3', class_='ipc-title__text')
                    if title_elem:
                        title = title_elem.get_text().split('. ', 1)[-1]  # 번호 제거
                        
                        # 영화 링크 추출
                        link_elem = item.find('a')
                        if link_elem:
                            movie_url = "https://www.imdb.com" + link_elem['href']
                            movies.append({'title': title, 'url': movie_url})
                            
                except Exception as e:
                    print(f"영화 정보 추출 중 오류: {e}")
                    continue
                    
            print(f"총 {len(movies)}개 영화 정보를 가져왔습니다.")
            return movies
            
        except Exception as e:
            print(f"Top 250 크롤링 중 오류 발생: {e}")
            return []
    
    def extract_actors_from_movie(self, movie_url):
        """개별 영화 페이지에서 주연 배우들만 정확히 추출"""
        try:
            time.sleep(1)  # 요청 간격 조절
            response = self.session.get(movie_url)
            response.raise_for_status()
            
            soup = BeautifulSoup(response.content, 'html.parser')
            actors = []
            
            # 더 정확한 캐스트 섹션 선택자들
            cast_selectors = [
                # 새로운 IMDB 디자인
                '[data-testid="title-cast-item__actor"]',
                '[data-testid="cast-member-link"]', 
                # Cast 섹션의 배우 링크만
                '.cast_list .primary_photo + td a',
                '.cast_list td[itemprop="actor"] a',
                # 백업 선택자 (배우만)
                'a[href*="/name/"]:not([href*="director"]):not([href*="writer"])'
            ]
            
            for selector in cast_selectors:
                actor_elements = soup.select(selector)
                if actor_elements:
                    for elem in actor_elements[:8]:  # 상위 8명만
                        actor_name = elem.get_text().strip()
                        # 감독, 작가 등 제외 키워드 필터링
                        exclude_keywords = ['Director', 'Writer', 'Producer', 'Cinematographer', 
                                          'Music', 'Editor', 'Production', 'Art Direction']
                        
                        if (actor_name and 
                            len(actor_name) > 2 and 
                            not any(keyword.lower() in actor_name.lower() for keyword in exclude_keywords)):
                            actors.append(actor_name)
                    break
            
            # Cast 섹션 전용 추가 확인
            cast_section = soup.find('section', {'data-testid': 'title-cast'}) or soup.find('table', class_='cast_list')
            if cast_section and len(actors) < 4:
                cast_links = cast_section.find_all('a', href=re.compile(r'/name/nm\d+/'))
                for link in cast_links[:6]:
                    name = link.get_text().strip()
                    if name and len(name) > 2 and name not in actors:
                        actors.append(name)
            
            return actors  # 모든 배우 반환 (감독/작가 필터링 완료)
            
        except Exception as e:
            print(f"배우 추출 중 오류 ({movie_url}): {e}")
            return []
    
    def get_actor_movies(self, actor_name):
        """특정 배우의 영화 목록 크롤링 (배우로 출연한 작품만)"""
        try:
            # 배우 이름을 IMDB 검색용으로 변환
            search_name = actor_name.replace(' ', '+')
            search_url = f"https://www.imdb.com/find?q={search_name}&s=nm"
            
            time.sleep(1)
            response = self.session.get(search_url)
            response.raise_for_status()
            
            soup = BeautifulSoup(response.content, 'html.parser')
            
            # 첫 번째 검색 결과의 배우 페이지로 이동
            actor_link = soup.find('a', class_='ipc-metadata-list-summary-item__t')
            if not actor_link:
                return []
                
            actor_url = "https://www.imdb.com" + actor_link['href']
            
            time.sleep(1)
            response = self.session.get(actor_url)
            soup = BeautifulSoup(response.content, 'html.parser')
            
            movies = []
            
            # Actor/Actress 섹션만 찾기 (감독 섹션 제외)
            filmography_sections = soup.find_all('div', {'data-testid': 'nm-flmg-actor'}) or \
                                 soup.find_all('div', {'data-testid': 'nm-flmg-actress'})
            
            if filmography_sections:
                for section in filmography_sections:
                    movie_links = section.find_all('a', href=re.compile(r'/title/tt\d+/'))
                    for link in movie_links[:25]:  # 최대 25개
                        movie_title = link.get_text().strip()
                        if movie_title and len(movie_title) > 1:
                            movies.append(movie_title)
            else:
                # 백업: 전체 페이지에서 영화 링크 찾되 필터링 강화
                all_movie_links = soup.find_all('a', href=re.compile(r'/title/tt\d+/'))
                
                for link in all_movie_links[:50]:
                    movie_title = link.get_text().strip()
                    # 감독 작품 필터링
                    parent_text = link.parent.get_text() if link.parent else ""
                    
                    if (movie_title and 
                        len(movie_title) > 1 and 
                        'director' not in parent_text.lower() and
                        'writer' not in parent_text.lower() and
                        'producer' not in parent_text.lower()):
                        movies.append(movie_title)
            
            return list(set(movies))  # 중복 제거
            
        except Exception as e:
            print(f"{actor_name} 영화 목록 크롤링 중 오류: {e}")
            return []
    
    def build_database(self):
        """데이터베이스 구축"""
        print("=== 영화 배우 케빈 베이컨 게임 ===")
        print("배우 50명 선정 기준: IMDB Top 25 영화의 주연 배우들")
        print()
        
        # 캐시 파일 확인
        cache_file = "actors_database.json"
        if os.path.exists(cache_file):
            print("기존 데이터베이스를 불러오는 중...")
            try:
                with open(cache_file, 'r', encoding='utf-8') as f:
                    cache_data = json.load(f)
                    self.actors_list = cache_data['actors']
                    
                    # DataFrame 구축
                    rows = []
                    for actor, movies in cache_data['data'].items():
                        for movie in movies:
                            rows.append({'actor': actor, 'movie': movie})
                    
                    self.actors_data = pd.DataFrame(rows)
                    print(f"데이터베이스 로딩 완료: {len(self.actors_list)}명 배우")
                    return
            except Exception as e:
                print(f"캐시 로딩 실패: {e}")
        
        print("새로운 데이터베이스를 구축하는 중...")
        
        # Top 250에서 영화들 가져오기
        movies = self.crawl_top_250()
        if not movies:
            print("영화 목록을 가져올 수 없습니다.")
            return
        
        # 각 영화에서 배우들 추출
        actor_count = defaultdict(int)
        actor_movies = defaultdict(set)
        
        print("영화별 배우 정보 수집 중...")
        for i, movie in enumerate(movies, 1):
            print(f"진행: {i}/{len(movies)} - {movie['title']}")
            actors = self.extract_actors_from_movie(movie['url'])
            
            for actor in actors:
                actor_count[actor] += 1
                actor_movies[actor].add(movie['title'])
        
        # 상위 50명 배우 선정 (출연 빈도 기준)
        top_actors = sorted(actor_count.items(), key=lambda x: x[1], reverse=True)[:50]
        self.actors_list = [actor for actor, count in top_actors]
        
        print(f"\n선정된 상위 50명 배우:")
        for i, (actor, count) in enumerate(top_actors, 1):
            print(f"{i:2d}. {actor} ({count}편)")
        
        # 선정된 배우들의 추가 영화 정보 수집
        print("\n선정된 배우들의 추가 영화 정보 수집 중...")
        rows = []
        final_data = {}
        
        for i, actor in enumerate(self.actors_list, 1):
            print(f"수집 중: {i}/50 - {actor}")
            
            # 기존 영화들
            existing_movies = list(actor_movies[actor])
            
            # 추가 영화들 크롤링
            additional_movies = self.get_actor_movies(actor)
            
            # 전체 영화 목록
            all_movies = list(set(existing_movies + additional_movies))
            
            # 영화 제목 정리 (통일성 확보)
            cleaned_movies = []
            for movie in all_movies:
                clean_title = self.clean_movie_title(movie)
                if clean_title and len(clean_title) > 1:
                    cleaned_movies.append(clean_title)
            
            final_data[actor] = list(set(cleaned_movies))  # 중복 제거
            
            # DataFrame용 데이터
            for movie in final_data[actor]:
                rows.append({'actor': actor, 'movie': movie})
        
        self.actors_data = pd.DataFrame(rows)
        
        # 캐시 저장
        cache_data = {
            'actors': self.actors_list,
            'data': final_data
        }
        with open(cache_file, 'w', encoding='utf-8') as f:
            json.dump(cache_data, f, ensure_ascii=False, indent=2)
        
        print(f"\n데이터베이스 구축 완료!")
        print(f"총 배우: {len(self.actors_list)}명")
        print(f"총 영화-배우 관계: {len(self.actors_data)}개")
    
    def find_shortest_path(self, actor1, actor2):
        """BFS로 두 배우 간 최단 경로 찾기"""
        if actor1 == actor2:
            return {'distance': 0, 'path': [actor1], 'movies': []}
        
        queue = deque([(actor1, [actor1], [])])
        visited = {actor1}
        
        while queue:
            current_actor, path, movies = queue.popleft()
            
            for neighbor, movie in self.graph[current_actor]:
                if neighbor == actor2:
                    return {
                        'distance': len(path),
                        'path': path + [neighbor],
                        'movies': movies + [movie]
                    }
                
                if neighbor not in visited:
                    visited.add(neighbor)
                    queue.append((neighbor, path + [neighbor], movies + [movie]))
        
        return {'distance': -1, 'path': [], 'movies': []}
    
    def expand_database_dynamically(self, target_actors):
        """특정 배우들의 공동 출연진을 데이터베이스에 추가"""
        print("연결 경로 최적화를 위해 추가 데이터 수집 중...")
        
        new_actors = set()
        new_rows = []
        
        # 타겟 배우들의 영화에서 공동 출연진 찾기
        for actor in target_actors:
            if actor in self.actors_list:
                actor_movies = self.actors_data[self.actors_data['actor'] == actor]['movie'].tolist()
                
                for movie in actor_movies[:10]:  # 주요 영화들만
                    # 해당 영화의 다른 출연진 크롤링
                    try:
                        search_url = f"https://www.imdb.com/find?q={movie.replace(' ', '+')}&s=tt"
                        time.sleep(1)
                        response = self.session.get(search_url)
                        
                        if response.status_code == 200:
                            soup = BeautifulSoup(response.content, 'html.parser')
                            movie_link = soup.find('a', class_='ipc-metadata-list-summary-item__t')
                            
                            if movie_link:
                                movie_url = "https://www.imdb.com" + movie_link['href']
                                cast = self.extract_actors_from_movie(movie_url)
                                
                                for cast_member in cast:
                                    if cast_member not in self.actors_list and cast_member not in new_actors:
                                        new_actors.add(cast_member)
                                        # 새 배우의 영화들도 수집 (기존 방식과 동일하게)
                                        actor_movies_new = self.get_actor_movies(cast_member)
                                        for movie_new in actor_movies_new:
                                            # 영화 제목 정리 (연도 정보 제거 등)
                                            clean_movie_title = self.clean_movie_title(movie_new)
                                            if clean_movie_title:
                                                new_rows.append({'actor': cast_member, 'movie': clean_movie_title})
                    except:
                        continue
        
        if new_rows:
            new_df = pd.DataFrame(new_rows)
            self.actors_data = pd.concat([self.actors_data, new_df], ignore_index=True)
            
            # 새로 추가된 배우들을 actors_list에 추가
            for new_actor in new_actors:
                if new_actor not in self.actors_list:
                    self.actors_list.append(new_actor)
            
            print(f"추가된 배우: {len(new_actors)}명, 추가된 관계: {len(new_rows)}개")
            
            # 새로 추가된 배우들 출력
            if new_actors:
                print("새로 추가된 배우들:")
                for i, actor in enumerate(sorted(new_actors), 1):
                    print(f"  {len(self.actors_list) - len(new_actors) + i}. {actor}")
        
        return len(new_actors) > 0
    
    def clean_movie_title(self, title):
        """영화 제목 정리 (연도 정보 제거, 통일성 확보)"""
        if not title:
            return ""
            
        # 연도 패턴 제거 (1990), [2020] 등
        import re
        title = re.sub(r'\s*[\(\[]?\d{4}[\)\]]?\s*', '', title)
        
        # 앞뒤 공백 제거
        title = title.strip()
        
        # TV 시리즈 표시 제거
        title = re.sub(r'\s*\(TV.*?\)', '', title)
        title = re.sub(r'\s*\(Video.*?\)', '', title)
        
        return title
        """특정 배우들의 공동 출연진을 데이터베이스에 추가"""
        print("연결 경로 최적화를 위해 추가 데이터 수집 중...")
        
        new_actors = set()
        new_rows = []
        
        # 타겟 배우들의 영화에서 공동 출연진 찾기
        for actor in target_actors:
            if actor in self.actors_list:
                actor_movies = self.actors_data[self.actors_data['actor'] == actor]['movie'].tolist()
                
                for movie in actor_movies[:10]:  # 주요 영화들만
                    # 해당 영화의 다른 출연진 크롤링
                    try:
                        search_url = f"https://www.imdb.com/find?q={movie.replace(' ', '+')}&s=tt"
                        time.sleep(1)
                        response = self.session.get(search_url)
                        
                        if response.status_code == 200:
                            soup = BeautifulSoup(response.content, 'html.parser')
                            movie_link = soup.find('a', class_='ipc-metadata-list-summary-item__t')
                            
                            if movie_link:
                                movie_url = "https://www.imdb.com" + movie_link['href']
                                cast = self.extract_actors_from_movie(movie_url)
                                
                                for cast_member in cast:
                                    if cast_member not in self.actors_list and cast_member not in new_actors:
                                        new_actors.add(cast_member)
                                        # 새 배우의 영화들도 수집
                                        actor_movies_new = self.get_actor_movies(cast_member)
                                        for movie_new in actor_movies_new:
                                            new_rows.append({'actor': cast_member, 'movie': movie_new})
                    except:
                        continue
        
        if new_rows:
            new_df = pd.DataFrame(new_rows)
            self.actors_data = pd.concat([self.actors_data, new_df], ignore_index=True)
            print(f"추가된 배우: {len(new_actors)}명, 추가된 관계: {len(new_rows)}개")
        
        return len(new_actors) > 0
    
    def build_graph(self):
        """배우 간 연결 그래프 구축"""
        self.graph = defaultdict(set)
        
        # 영화별로 출연 배우들을 그룹화
        movie_actors = self.actors_data.groupby('movie')['actor'].apply(list).to_dict()
        
        # 같은 영화에 출연한 배우들끼리 연결
        for movie, actors in movie_actors.items():
            for i in range(len(actors)):
                for j in range(i + 1, len(actors)):
                    actor1, actor2 = actors[i], actors[j]
                    self.graph[actor1].add((actor2, movie))
                    self.graph[actor2].add((actor1, movie))
    
    def find_shortest_path_enhanced(self, actor1, actor2):
        """향상된 최단 경로 찾기 (동적 확장 포함)"""
        # 첫 번째 시도: 기존 데이터로 경로 찾기
        result = self.find_shortest_path(actor1, actor2)
        
        print(f"첫 번째 시도 결과: 거리 {result['distance']}")
        
        # 연결되지 않거나 거리가 4 이상이면 데이터 확장 시도
        if result['distance'] == -1:
            print("❌ 연결되지 않음 - 추가 데이터 수집을 시도합니다...")
            
            # 두 배우의 공동 출연진 데이터 확장
            expanded = self.expand_database_dynamically([actor1, actor2])
            
            if expanded:
                print("✅ 데이터 확장 완료 - 재계산 중...")
                # 그래프 재구축
                self.build_graph()
                # 다시 경로 찾기
                new_result = self.find_shortest_path(actor1, actor2)
                if new_result['distance'] != -1:
                    print(f"✅ 확장 후 결과: 거리 {new_result['distance']}")
                    return new_result
                else:
                    print("❌ 확장 후에도 연결되지 않음")
            else:
                print("❌ 추가 데이터를 찾을 수 없음")
        
        elif result['distance'] >= 4:
            print(f"⚠️  거리 {result['distance']} - 더 짧은 경로를 찾기 위해 추가 데이터 수집...")
            
            # 두 배우의 공동 출연진 데이터 확장
            expanded = self.expand_database_dynamically([actor1, actor2])
            
            if expanded:
                print("✅ 데이터 확장 완료 - 재계산 중...")
                # 그래프 재구축
                self.build_graph()
                # 다시 경로 찾기
                new_result = self.find_shortest_path(actor1, actor2)
                if new_result['distance'] != -1 and new_result['distance'] < result['distance']:
                    print(f"✅ 더 짧은 경로 발견: {result['distance']} → {new_result['distance']}")
                    return new_result
                else:
                    print(f"ℹ️  기존 경로가 최선: 거리 {result['distance']}")
            else:
                print("❌ 추가 데이터를 찾을 수 없음")
        
        else:
            print(f"✅ 양호한 거리: {result['distance']}")
        
        return result
    
    def run_game(self):
        """게임 실행"""
        if not self.actors_list:
            print("데이터베이스가 구축되지 않았습니다.")
            return
        
        self.build_graph()
        print("\n" + "="*60)
        print("선택 가능한 배우 목록")
        print("="*60)
        
        # 3열로 배우 목록 출력
        for i in range(0, len(self.actors_list), 3):
            line = ""
            for j in range(3):
                if i + j < len(self.actors_list):
                    actor_num = i + j + 1
                    actor_name = self.actors_list[i + j]
                    line += f"{actor_num:2d}. {actor_name:<20}"
            print(line)
        
        print("\n" + "="*60)
        
        while True:
            try:
                print("\\n연결 관계를 확인할 두 배우의 번호를 입력하세요.")
                print("(종료하려면 0을 입력)")
                
                actor1_num = int(input("첫 번째 배우 번호: "))
                if actor1_num == 0:
                    print("게임을 종료합니다.")
                    break
                
                actor2_num = int(input("두 번째 배우 번호: "))
                if actor2_num == 0:
                    print("게임을 종료합니다.")
                    break
                
                if not (1 <= actor1_num <= len(self.actors_list)) or not (1 <= actor2_num <= len(self.actors_list)):
                    print("올바른 번호를 입력해주세요.")
                    continue
                
                actor1 = self.actors_list[actor1_num - 1]
                actor2 = self.actors_list[actor2_num - 1]
                
                print(f"\\n계산 중: {actor1} ↔ {actor2}")
                result = self.find_shortest_path_enhanced(actor1, actor2)  # 향상된 버전 활성화
                
                if result['distance'] == -1:
                    print("두 배우는 연결되지 않았습니다.")
                else:
                    print(f"\\n결과: 거리는 {result['distance']}입니다.")
                    
                    # 경로 출력
                    path_str = ""
                    for i in range(len(result['path'])):
                        path_str += result['path'][i]
                        if i < len(result['movies']):
                            path_str += f" → ({result['movies'][i]}) → "
                    
                    print(f"경로: {path_str}")
                
            except ValueError:
                print("숫자를 입력해주세요.")
            except KeyboardInterrupt:
                print("\\n게임을 종료합니다.")
                break
            except Exception as e:
                print(f"오류 발생: {e}")

def main():
    game = MovieBaconGame()
    
    # 데이터베이스 구축
    game.build_database()
    
    # 게임 실행
    game.run_game()

if __name__ == "__main__":
    main()