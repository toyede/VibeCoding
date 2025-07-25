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
        """IMDB Top 250 ��ȭ ��� ũ�Ѹ�"""
        print("IMDB Top 250 ��ȭ ����� �������� ��...")
        
        url = "https://www.imdb.com/chart/top/"
        try:
            response = self.session.get(url)
            response.raise_for_status()
            
            soup = BeautifulSoup(response.content, 'html.parser')
            movies = []
            
            # IMDB Top 250 �������� ��ȭ �׸�� ã��
            movie_items = soup.find_all('li', class_='ipc-metadata-list-summary-item')
            
            for item in movie_items[:50]:  # ó�� �ӵ��� ���� ���� 50����
                try:
                    # ��ȭ ���� ����
                    title_elem = item.find('h3', class_='ipc-title__text')
                    if title_elem:
                        title = title_elem.get_text().split('. ', 1)[-1]  # ��ȣ ����
                        
                        # ��ȭ ��ũ ����
                        link_elem = item.find('a')
                        if link_elem:
                            movie_url = "https://www.imdb.com" + link_elem['href']
                            movies.append({'title': title, 'url': movie_url})
                            
                except Exception as e:
                    print(f"��ȭ ���� ���� �� ����: {e}")
                    continue
                    
            print(f"�� {len(movies)}�� ��ȭ ������ �����Խ��ϴ�.")
            return movies
            
        except Exception as e:
            print(f"Top 250 ũ�Ѹ� �� ���� �߻�: {e}")
            return []
    
    def extract_actors_from_movie(self, movie_url):
        """���� ��ȭ ���������� �ֿ� ���� ����"""
        try:
            time.sleep(1)  # ��û ���� ����
            response = self.session.get(movie_url)
            response.raise_for_status()
            
            soup = BeautifulSoup(response.content, 'html.parser')
            actors = []
            
            # ĳ��Ʈ ���ǿ��� ���� ã�� (���� ������ �õ�)
            cast_selectors = [
                'a[data-testid="title-cast-item__actor"]',
                '.cast_list a',
                '[data-testid="title-cast-item"] a'
            ]
            
            for selector in cast_selectors:
                actor_elements = soup.select(selector)
                if actor_elements:
                    for elem in actor_elements[:6]:  # ���� 6��
                        actor_name = elem.get_text().strip()
                        if actor_name and len(actor_name) > 2:
                            actors.append(actor_name)
                    break
            
            return actors[:4]  # �ִ� 4���� �ֿ���
            
        except Exception as e:
            print(f"��� ���� �� ���� ({movie_url}): {e}")
            return []
    
    def get_actor_movies(self, actor_name):
        """Ư�� ����� ��ȭ ��� ũ�Ѹ� (���� ����)"""
        try:
            # ��� �̸��� IMDB �˻������� ��ȯ
            search_name = actor_name.replace(' ', '+')
            search_url = f"https://www.imdb.com/find?q={search_name}&s=nm"
            
            time.sleep(1)
            response = self.session.get(search_url)
            response.raise_for_status()
            
            soup = BeautifulSoup(response.content, 'html.parser')
            
            # ù ��° �˻� ����� ��� �������� �̵�
            actor_link = soup.find('a', class_='ipc-metadata-list-summary-item__t')
            if not actor_link:
                return []
                
            actor_url = "https://www.imdb.com" + actor_link['href']
            
            time.sleep(1)
            response = self.session.get(actor_url)
            soup = BeautifulSoup(response.content, 'html.parser')
            
            movies = []
            # ��ȭ ��� ���� (���� ����)
            movie_links = soup.find_all('a', href=re.compile(r'/title/tt\d+/'))
            
            for link in movie_links[:15]:  # �ִ� 15�� ��ȭ
                movie_title = link.get_text().strip()
                if movie_title and len(movie_title) > 1:
                    movies.append(movie_title)
            
            return list(set(movies))  # �ߺ� ����
            
        except Exception as e:
            print(f"{actor_name} ��ȭ ��� ũ�Ѹ� �� ����: {e}")
            return []
    
    def build_database(self):
        """�����ͺ��̽� ����"""
        print("=== ��ȭ ��� �ɺ� ������ ���� ===")
        print("��� 50�� ���� ����: IMDB Top 250 ��ȭ�� �ֿ� ����")
        print()
        
        # ĳ�� ���� Ȯ��
        cache_file = "actors_database.json"
        if os.path.exists(cache_file):
            print("���� �����ͺ��̽��� �ҷ����� ��...")
            try:
                with open(cache_file, 'r', encoding='utf-8') as f:
                    cache_data = json.load(f)
                    self.actors_list = cache_data['actors']
                    
                    # DataFrame ����
                    rows = []
                    for actor, movies in cache_data['data'].items():
                        for movie in movies:
                            rows.append({'actor': actor, 'movie': movie})
                    
                    self.actors_data = pd.DataFrame(rows)
                    print(f"�����ͺ��̽� �ε� �Ϸ�: {len(self.actors_list)}�� ���")
                    return
            except Exception as e:
                print(f"ĳ�� �ε� ����: {e}")
        
        print("���ο� �����ͺ��̽��� �����ϴ� ��...")
        
        # Top 250���� ��ȭ�� ��������
        movies = self.crawl_top_250()
        if not movies:
            print("��ȭ ����� ������ �� �����ϴ�.")
            return
        
        # �� ��ȭ���� ���� ����
        actor_count = defaultdict(int)
        actor_movies = defaultdict(set)
        
        print("��ȭ�� ��� ���� ���� ��...")
        for i, movie in enumerate(movies, 1):
            print(f"����: {i}/{len(movies)} - {movie['title']}")
            actors = self.extract_actors_from_movie(movie['url'])
            
            for actor in actors:
                actor_count[actor] += 1
                actor_movies[actor].add(movie['title'])
        
        # ���� 50�� ��� ���� (�⿬ �� ����)
        top_actors = sorted(actor_count.items(), key=lambda x: x[1], reverse=True)[:50]
        self.actors_list = [actor for actor, count in top_actors]
        
        print(f"\n������ ���� 50�� ���:")
        for i, (actor, count) in enumerate(top_actors, 1):
            print(f"{i:2d}. {actor} ({count}��)")
        
        # ������ ������ �߰� ��ȭ ���� ����
        print("\n������ ������ �߰� ��ȭ ���� ���� ��...")
        rows = []
        final_data = {}
        
        for i, actor in enumerate(self.actors_list, 1):
            print(f"���� ��: {i}/50 - {actor}")
            
            # ���� ��ȭ��
            existing_movies = list(actor_movies[actor])
            
            # �߰� ��ȭ�� ũ�Ѹ�
            additional_movies = self.get_actor_movies(actor)
            
            # ��ü ��ȭ ���
            all_movies = list(set(existing_movies + additional_movies))
            final_data[actor] = all_movies
            
            # DataFrame�� ������
            for movie in all_movies:
                rows.append({'actor': actor, 'movie': movie})
        
        self.actors_data = pd.DataFrame(rows)
        
        # ĳ�� ����
        cache_data = {
            'actors': self.actors_list,
            'data': final_data
        }
        with open(cache_file, 'w', encoding='utf-8') as f:
            json.dump(cache_data, f, ensure_ascii=False, indent=2)
        
        print(f"\n�����ͺ��̽� ���� �Ϸ�!")
        print(f"�� ���: {len(self.actors_list)}��")
        print(f"�� ��ȭ-��� ����: {len(self.actors_data)}��")
    
    def build_graph(self):
        """��� �� ���� �׷��� ����"""
        self.graph = defaultdict(set)
        
        # ��ȭ���� �⿬ ������ �׷�ȭ
        movie_actors = self.actors_data.groupby('movie')['actor'].apply(list).to_dict()
        
        # ���� ��ȭ�� �⿬�� ���鳢�� ����
        for movie, actors in movie_actors.items():
            for i in range(len(actors)):
                for j in range(i + 1, len(actors)):
                    actor1, actor2 = actors[i], actors[j]
                    self.graph[actor1].add((actor2, movie))
                    self.graph[actor2].add((actor1, movie))
    
    def find_shortest_path(self, actor1, actor2):
        """BFS�� �� ��� �� �ִ� ��� ã��"""
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
    
    def run_game(self):
        """���� ����"""
        if not self.actors_list:
            print("�����ͺ��̽��� ������� �ʾҽ��ϴ�.")
            return
        
        self.build_graph()
        print("\n" + "="*60)
        print("���� ������ ��� ���")
        print("="*60)
        
        # 3���� ��� ��� ���
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
                print("\\n���� ���踦 Ȯ���� �� ����� ��ȣ�� �Է��ϼ���.")
                print("(�����Ϸ��� 0�� �Է�)")
                
                actor1_num = int(input("ù ��° ��� ��ȣ: "))
                if actor1_num == 0:
                    print("������ �����մϴ�.")
                    break
                
                actor2_num = int(input("�� ��° ��� ��ȣ: "))
                if actor2_num == 0:
                    print("������ �����մϴ�.")
                    break
                
                if not (1 <= actor1_num <= len(self.actors_list)) or not (1 <= actor2_num <= len(self.actors_list)):
                    print("�ùٸ� ��ȣ�� �Է����ּ���.")
                    continue
                
                actor1 = self.actors_list[actor1_num - 1]
                actor2 = self.actors_list[actor2_num - 1]
                
                print(f"\\n��� ��: {actor1} �� {actor2}")
                result = self.find_shortest_path(actor1, actor2)
                
                if result['distance'] == -1:
                    print("�� ���� ������� �ʾҽ��ϴ�.")
                else:
                    print(f"\\n���: �Ÿ��� {result['distance']}�Դϴ�.")
                    
                    # ��� ���
                    path_str = ""
                    for i in range(len(result['path'])):
                        path_str += result['path'][i]
                        if i < len(result['movies']):
                            path_str += f" �� ({result['movies'][i]}) �� "
                    
                    print(f"���: {path_str}")
                
            except ValueError:
                print("���ڸ� �Է����ּ���.")
            except KeyboardInterrupt:
                print("\\n������ �����մϴ�.")
                break
            except Exception as e:
                print(f"���� �߻�: {e}")

def main():
    game = MovieBaconGame()
    
    # �����ͺ��̽� ����
    game.build_database()
    
    # ���� ����
    game.run_game()

if __name__ == "__main__":
    main()