import pandas as pd
import time
from selenium import webdriver
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from webdriver_manager.chrome import ChromeDriverManager
import re

def get_filmography_selenium(actor_name):
    """
    IMDb에서 Selenium을 사용하여 배우의 필모그래피를 가져와 pandas DataFrame으로 반환합니다.

    Args:
        actor_name (str): 검색할 배우의 이름.

    Returns:
        pandas.DataFrame: 'Actor'와 'Movie_Title' 컬럼을 가진 DataFrame.
                           오류 발생 시 None을 반환합니다.
    """
    driver = None
    try:
        options = webdriver.ChromeOptions()
        # options.add_argument('--headless') # 브라우저 창을 띄우지 않음 옵션 제거
        options.add_argument('--no-sandbox')
        options.add_argument('--disable-dev-shm-usage')
        options.add_argument("user-agent=Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36")

        service = Service(ChromeDriverManager().install())
        driver = webdriver.Chrome(service=service, options=options)
        wait = WebDriverWait(driver, 60) # 대기 시간을 60초로 늘림

        # 1. 배우 검색 페이지로 이동
        search_url = f"https://www.imdb.com/find?q={actor_name}"
        driver.get(search_url)
        time.sleep(3) # 페이지 로드를 위한 대기

        # 2. 검색 결과에서 배우 페이지 링크 찾기
        result_section = wait.until(
            EC.presence_of_element_located((By.CSS_SELECTOR, "section[data-testid='find-results-section-name']"))
        )
        actor_link_tag = result_section.find_element(By.CSS_SELECTOR, "a[href*='/name/nm']")
        actor_url = actor_link_tag.get_attribute('href')

        # 3. 배우 페이지로 이동 (모든 크레딧을 보여주는 URL로 변경)
        actor_id = actor_url.split('/')[4] # nmXXXXXXX 추출
        full_credits_url = f"https://www.imdb.com/name/{actor_id}/?showAllCredits=true"
        driver.get(full_credits_url)
        time.sleep(5) # 페이지 로드를 위한 충분한 대기 시간

        # 4. (중요) 쿠키 동의 버튼이 있다면 클릭
        try:
            cookie_button = wait.until(EC.element_to_be_clickable((By.CSS_SELECTOR, "button[data-testid='accept-button']")))
            cookie_button.click()
            time.sleep(2) # 클릭 후 페이지 안정화를 위한 대기
        except Exception:
            print("쿠키 동의 버튼을 찾지 못했거나 클릭할 수 없습니다. 계속 진행합니다.")

        # 5. 페이지 스크롤 다운하여 모든 콘텐츠 로드 유도
        last_height = driver.execute_script("return document.body.scrollHeight")
        while True:
            driver.execute_script("window.scrollTo(0, document.body.scrollHeight);")
            time.sleep(2) # 스크롤 후 로드 대기
            new_height = driver.execute_script("return document.body.scrollHeight")
            if new_height == last_height:
                break
            last_height = new_height

        # 6. 영화 제목 링크가 나타날 때까지 대기
        wait.until(EC.presence_of_element_located((By.CSS_SELECTOR, "a.ipc-md-link.ipc-md-link--entity")))

        movies = []
        
        # 모든 필모그래피 섹션에서 영화 제목을 찾습니다.
        # data-testid가 'nm_flmg_sec_'으로 시작하는 section 태그를 찾습니다.
        filmography_sections = driver.find_elements(By.CSS_SELECTOR, "section[data-testid^='nm_flmg_sec_']")

        if not filmography_sections:
             print("필모그래피 섹션을 찾을 수 없습니다.")
             # 필모그래피가 없는 경우에도 HTML을 저장하여 디버깅에 활용
             with open("imdb_page_source_no_filmography.html", "w", encoding="utf-8") as f:
                 f.write(driver.page_source)
             print("필모그래피가 없는 페이지의 HTML 소스를 'imdb_page_source_no_filmography.html'에 저장했습니다.")
             return pd.DataFrame({'Actor': [actor_name], 'Movie_Title': [None]})

        for section in filmography_sections:
            # 각 섹션 내에서 영화 제목 링크(a 태그)를 찾습니다.
            movie_tags = section.find_elements(By.CSS_SELECTOR, 'a.ipc-md-link.ipc-md-link--entity')
            for tag in movie_tags:
                title = tag.text.strip()
                href = tag.get_attribute('href')
                # 영화 제목 링크인지 확인 (actor 페이지 링크 등 제외)
                if title and href and "/title/tt" in href:
                    if title not in movies:
                        movies.append(title)

        if not movies:
            print(f"'{actor_name}'의 필모그래피를 찾았지만, 내용이 비어있습니다.")
            return pd.DataFrame({'Actor': [actor_name], 'Movie_Title': [None]})

        df = pd.DataFrame({
            'Actor': actor_name,
            'Movie_Title': movies
        })
        
        return df

    except Exception as e:
        print(f"오류 발생: {e}")
        if driver:
            with open("imdb_error_page.html", "w", encoding="utf-8") as f:
                f.write(driver.page_source)
            print("오류 발생 시점의 HTML을 'imdb_error_page.html'에 저장했습니다.")
        return None
    finally:
        if driver:
            driver.quit()

if __name__ == '__main__':
    actor = "Kevin Bacon"
    filmography_df = get_filmography_selenium(actor)
    
    if filmography_df is not None and not filmography_df.empty:
        print(f"\n'{actor}'의 필모그래피:")
        print(filmography_df)
        
        output_filename = f'{actor.replace(" ", "_").lower()}_filmography.csv'
        filmography_df.to_csv(output_filename, index=False, encoding='utf-8-sig')
        print(f"\n결과가 {output_filename} 파일로 저장되었습니다.")
    elif filmography_df is not None:
        print(f"'{actor}'의 필모그래피를 찾았지만, 내용이 비어있습니다.")
