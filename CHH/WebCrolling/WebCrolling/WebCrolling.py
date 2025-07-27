
import re
import time
import requests
import pandas as pd
from bs4 import BeautifulSoup
from collections import deque

# ─── 설정 ─────────────────────────────────────────────────
BASE_URL = "https://www.imdb.com"
HEADERS = {
    "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64)"
}
session = requests.Session()
session.headers.update(HEADERS)


# ─── 배우 페이지 찾기 ─────────────────────────────────────────
def find_actor_url(actor_name: str) -> str:
    """
    IMDb 검색 결과에서 배우 페이지 URL을 찾아 반환.
    찾으면 [FOUND] 메시지 출력.
    """
    params = {'q': actor_name, 's': 'nm'}
    resp = session.get(f"{BASE_URL}/find", params=params)
    resp.raise_for_status()
    soup = BeautifulSoup(resp.text, "html.parser")

    selectors = [
        "td.result_text a[href^='/name/']",
        "a.ipc-metadata-list-summary-item__t[href^='/name/']",
        "[data-testid='find-result'] a[href^='/name/']",
    ]
    for sel in selectors:
        link = soup.select_one(sel)
        if link:
            href = link["href"].split('?', 1)[0]
            url = BASE_URL + href
            print(f"[FOUND] '{actor_name}' 페이지 URL: {url}")
            return url

    # 마지막 보장용 regex
    link = soup.find("a", href=re.compile(r"^/name/nm\d+"))
    if link:
        href = link["href"].split('?', 1)[0]
        url = BASE_URL + href
        print(f"[FOUND] (fallback) '{actor_name}' 페이지 URL: {url}")
        return url

    raise ValueError(f"'{actor_name}' 검색 결과에서 배우 페이지를 찾을 수 없습니다.")


def find_actor_id(actor_name: str) -> str:
    """
    find_actor_url()로 URL을 얻어 nmXXXXXXX 형태의 ID를 반환.
    """
    url = find_actor_url(actor_name)
    match = re.search(r"/name/(nm\d+)", url)
    if not match:
        raise ValueError(f"'{actor_name}' 페이지 URL에서 ID를 추출할 수 없습니다.")
    return match.group(1)


# ─── 필모그래피(영화 목록) 추출 ─────────────────────────────────
def get_films_by_actor(actor_id: str) -> pd.DataFrame:
    url = f"{BASE_URL}/name/{actor_id}/"
    resp = session.get(url); resp.raise_for_status()
    soup = BeautifulSoup(resp.text, "html.parser")

    films = []
    for row in soup.select("div.filmo-category-section div.filmo-row"):
        if any(tag in row.get("id", "") for tag in ("actor", "actress")):
            a = row.select_one("b a[href^='/title/tt']")
            if a and (m := re.search(r"/title/(tt\d+)", a["href"])):
                films.append({
                    "film_id": m.group(1),
                    "film_title": a.text.strip()
                })
    return pd.DataFrame(films)


# ─── 영화 출연진 추출 ─────────────────────────────────────────
def get_cast_by_film(film_id: str) -> pd.DataFrame:
    url = f"{BASE_URL}/title/{film_id}/fullcredits"
    resp = session.get(url); resp.raise_for_status()
    soup = BeautifulSoup(resp.text, "html.parser")

    cast = []
    for a in soup.select("table.cast_list tr td.primary_photo + td a[href^='/name/']"):
        name = a.text.strip()
        if (m := re.search(r"/name/(nm\d+)", a["href"])):
            cast.append({"actor_id": m.group(1), "actor_name": name})
    return pd.DataFrame(cast)


# ─── ID → 이름/제목 매핑 ───────────────────────────────────────
def get_actor_name(actor_id: str) -> str:
    url = f"{BASE_URL}/name/{actor_id}/"
    resp = session.get(url); resp.raise_for_status()
    soup = BeautifulSoup(resp.text, "html.parser")
    h = soup.select_one("h1.header span.itemprop") or soup.select_one("h1 span")
    return h.text.strip() if h else actor_id

def get_film_title(film_id: str) -> str:
    url = f"{BASE_URL}/title/{film_id}/"
    resp = session.get(url); resp.raise_for_status()
    soup = BeautifulSoup(resp.text, "html.parser")
    h = soup.select_one("div.title_wrapper h1")
    return (h.contents[0].strip() if h else film_id)


# ─── 경로 가독화 & 거리 계산 ────────────────────────────────────
def build_readable_path(raw_path: list):
    distance = 0
    readable = []
    for node in raw_path:
        if node.startswith("nm"):
            readable.append(get_actor_name(node))
        else:
            distance += 1
            readable.append(f"▶ {get_film_title(node)}")
    return distance, readable


def kevin_bacon_distance(actor_a: str, actor_b: str):
    start_id = find_actor_id(actor_a)
    target_id = find_actor_id(actor_b)

    queue = deque([[start_id]])
    visited_actors = {start_id}
    visited_films = set()

    while queue:
        path = queue.popleft()
        current = path[-1]

        films_df = get_films_by_actor(current)
        for _, film in films_df.iterrows():
            fid = film["film_id"]
            if fid in visited_films:
                continue
            visited_films.add(fid)

            cast_df = get_cast_by_film(fid)
            for _, actor in cast_df.iterrows():
                aid = actor["actor_id"]
                if aid in visited_actors:
                    continue
                visited_actors.add(aid)

                new_path = path + [fid, aid]
                if aid == target_id:
                    return build_readable_path(new_path)
                queue.append(new_path)

        time.sleep(1)  # 요청 과부하 방지

    raise ValueError(f"'{actor_a}'와 '{actor_b}' 사이의 연결 고리를 찾을 수 없습니다.")


# ─── 실행부 ─────────────────────────────────────────────────
if __name__ == "__main__":
    inp = input("두 배우 이름을 영어로 입력하세요 (예: Tom Hanks & Kevin Bacon): ")
    a, b = [s.strip() for s in inp.split("&", 1)]
    dist, path = kevin_bacon_distance(a, b)
    print(f"\n{a} ↔ {b} 의 Kevin Bacon 거리: {dist}")
    print("경로:")
    for step in path:
        print(step)
