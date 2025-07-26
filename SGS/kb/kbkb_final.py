import time
import pandas as pd
from collections import deque

# Selenium 관련 모듈
from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.common.keys import Keys
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from webdriver_manager.chrome import ChromeDriverManager
from selenium.common.exceptions import TimeoutException

# BeautifulSoup 관련 모듈
from bs4 import BeautifulSoup

def setup_driver():
    """Chrome 웹 드라이버를 설정하고 반환합니다."""
    service = Service(ChromeDriverManager().install())
    options = webdriver.ChromeOptions()
    options.add_argument('--headless')
    options.add_argument('--log-level=3')
    # 일반 사용자처럼 보이게 하는 User-Agent 추가
    options.add_argument('user-agent=Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/123.0.0.0 Safari/537.36')
    options.add_experimental_option('excludeSwitches', ['enable-logging'])
    driver = webdriver.Chrome(service=service, options=options)
    return driver

def get_actor_url(driver, actor_name):
    """IMDb에서 배우 이름을 검색하여 해당 배우의 URL을 반환합니다."""
    print(f"'{actor_name}' 배우의 IMDb 페이지를 검색 중...")
    try:
        driver.get("https://www.imdb.com")
        # 대기 시간을 30초로 늘림
        search_box = WebDriverWait(driver, 30).until(
            EC.presence_of_element_located((By.ID, "suggestion-search"))
        )
        search_box.clear()
        search_box.send_keys(actor_name)
        search_box.send_keys(Keys.RETURN)

        result_link = WebDriverWait(driver, 30).until(
            EC.element_to_be_clickable((By.CSS_SELECTOR, "section[data-testid='find-results-section-name'] a.ipc-metadata-list-summary-item__t"))
        )
        actor_url = result_link.get_attribute('href')
        print(f"'{actor_name}' 배우의 페이지를 찾았습니다: {actor_url}")
        return actor_url
    except TimeoutException:
        print(f"'{actor_name}' 배우 검색 중 페이지 로딩 시간 초과")
        return None
    except Exception as e:
        print(f"'{actor_name}' 배우를 찾지 못했습니다. 이름을 다시 확인해주세요.")
        return None

def get_filmography(driver, actor_url):
    """배우의 필모그래피 페이지에서 (영화 제목, URL) 목록을 반환합니다."""
    print(f"필모그래피를 가져오는 중...")
    filmography = []
    try:
        driver.get(actor_url)
        WebDriverWait(driver, 30).until(
            EC.presence_of_element_located((By.CSS_SELECTOR, "div.filmo-category-section"))
        )
        soup = BeautifulSoup(driver.page_source, 'html.parser')
        
        for link in soup.select('div.filmo-row b a'):
            href = link.get('href')
            title = link.get_text(strip=True)
            if href and href.startswith('/title/'):
                filmography.append((title, "https://www.imdb.com" + href))
        
        print(f"총 {len(filmography)}개의 작품을 찾았습니다.")
        return filmography
    except TimeoutException:
        print(f"필모그래피 페이지 로딩 시간 초과 ({actor_url})")
        return []
    except Exception as e:
        print(f"필모그래피를 가져오는 데 실패했습니다: {e}")
        return []

def get_cast(driver, movie_url, movie_title):
    """영화/TV 쇼의 전체 출연진 목록을 반환합니다."""
    cast_list = []
    try:
        full_credits_url = movie_url.split('?')[0].strip() + "/fullcredits"
        driver.get(full_credits_url)
        
        cast_table_element = WebDriverWait(driver, 30).until(
            EC.presence_of_element_located((By.CSS_SELECTOR, "table.cast_list"))
        )
        soup = BeautifulSoup(cast_table_element.get_attribute('outerHTML'), 'html.parser')
        
        for row in soup.find_all('tr', class_=['odd', 'even']):
            cells = row.find_all('td')
            if len(cells) > 1 and cells[1].a:
                actor_name = cells[1].a.get_text(strip=True)
                if actor_name:
                    cast_list.append(actor_name)
    except TimeoutException:
        print(f"  - 영화 '{movie_title}'의 출연진 페이지 로딩 시간 초과")
    except Exception as e:
        print(f"  - 영화 '{movie_title}'의 출연진을 가져오는 중 오류 발생: {e}")
    return cast_list

def find_connection(start_actor, end_actor):
    """너비 우선 탐색(BFS)을 사용하여 두 배우 간의 최단 경로를 찾습니다."""
    driver = setup_driver()
    
    try:
        # 한 번 찾은 배우의 URL을 저장하여 반복 검색 방지
        visited_actor_urls = {}

        start_url = get_actor_url(driver, start_actor)
        if start_url:
            visited_actor_urls[start_actor.lower()] = start_url
        
        end_url = get_actor_url(driver, end_actor)

        if not start_url or not end_url:
            return None

        queue = deque([[start_actor]])
        visited_actors = {start_actor.lower()}

        while queue:
            path = queue.popleft()
            current_actor = path[-1]

            print(f"\n--- 현재 탐색 중인 배우: {current_actor} (경로 깊이: {len(path)//2}) ---")

            # URL 캐시 확인
            actor_url = visited_actor_urls.get(current_actor.lower())
            if not actor_url:
                actor_url = get_actor_url(driver, current_actor)
                if actor_url:
                    visited_actor_urls[current_actor.lower()] = actor_url
            
            if not actor_url:
                continue

            filmography = get_filmography(driver, actor_url)
            
            for movie_title, movie_url in filmography[:30]: 
                print(f"  -> 영화 '{movie_title}'의 출연진을 확인합니다.")
                
                cast = get_cast(driver, movie_url, movie_title)
                
                # 대소문자 구분 없이 목표 배우 확인
                for actor_in_cast in cast:
                    if actor_in_cast.lower() == end_actor.lower():
                        print(f"\n!!! '{movie_title}'에서 '{actor_in_cast}'를 찾았습니다! !!!")
                        final_path = path + [movie_title, actor_in_cast]
                        return final_path

                for co_star in cast:
                    if co_star.lower() not in visited_actors:
                        visited_actors.add(co_star.lower())
                        new_path = path + [movie_title, co_star]
                        queue.append(new_path)

    except Exception as e:
        print(f"탐색 중 심각한 오류가 발생했습니다: {e}")
    finally:
        print("탐색을 종료합니다.")
        driver.quit()

    return None

def main():
    actor1 = input("첫 번째 배우의 이름을 입력하세요 (예: Leonardo DiCaprio): ")
    actor2 = input("두 번째 배우의 이름을 입력하세요 (예: Tom Hanks): ")

    if actor1.lower() == actor2.lower():
        print("같은 배우를 입력했습니다.")
        return

    start_time = time.time()
    result_path = find_connection(actor1, actor2)
    end_time = time.time()

    print(f"\n--- 탐색 완료 (소요 시간: {end_time - start_time:.2f}초) ---")

    if result_path:
        data = []
        for i in range(0, len(result_path) - 2, 2):
            data.append({
                '출발 배우': result_path[i],
                '작품': result_path[i+1],
                '도착 배우': result_path[i+2]
            })
        
        df = pd.DataFrame(data)
        bacon_number = len(df)
        print(f"\n'{actor1}'와(과) '{actor2}'의 케빈 베이컨 단계는 {bacon_number}입니다.")
        print("\n[연결 경로]")
        print(df.to_string(index=False))
    else:
        print(f"\n'{actor1}'와(과) '{actor2}' 사이의 연결고리를 찾지 못했습니다.")

if __name__ == "__main__":
    main()