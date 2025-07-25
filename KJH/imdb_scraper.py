

import requests
from bs4 import BeautifulSoup
import pandas as pd
import re

def get_movie_data():
    # IMDb Top 250 movies page
    url = "https://www.imdb.com/chart/top"
    headers = {
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36"
    }

    try:
        response = requests.get(url, headers=headers)
        response.raise_for_status()  # Raise an exception for bad status codes
    except requests.exceptions.RequestException as e:
        print(f"Error fetching main page: {e}")
        return None

    soup = BeautifulSoup(response.content, "html.parser")
    
    movie_elements = soup.select("main ul li div.ipc-metadata-list-summary-item__c")
    
    movie_data = []
    
    # Limit to top 10 movies for this example
    for element in movie_elements[:10]:
        try:
            # Extract movie title
            title_element = element.select_one("h3.ipc-title__text")
            # The text is in the format "1. The Shawshank Redemption". We use regex to get only the title.
            title_match = re.match(r'\d+\.\s*(.*)', title_element.text.strip())
            if not title_match:
                continue
            title = title_match.group(1)

            # Extract movie page link
            link_element = element.select_one("a.ipc-title-link-wrapper")
            movie_url = "https://www.imdb.com" + link_element['href']

            # Visit movie page to get cast
            movie_response = requests.get(movie_url, headers=headers)
            movie_response.raise_for_status()
            movie_soup = BeautifulSoup(movie_response.content, "html.parser")
            
            # Extract actor names
            cast_elements = movie_soup.select('[data-testid="title-cast-item__actor"]')
            actors = [actor.text.strip() for actor in cast_elements[:5]] # Get top 5 actors

            movie_data.append({"title": title, "actors": ", ".join(actors)})
            print(f"Scraped: {title}")

        except requests.exceptions.RequestException as e:
            print(f"Error fetching movie page for {title}: {e}")
            continue
        except Exception as e:
            print(f"Error processing movie entry: {e}")
            continue

    return pd.DataFrame(movie_data)

if __name__ == "__main__":
    print("Starting IMDb movie scraping...")
    df = get_movie_data()
    if df is not None and not df.empty:
        # Save to CSV
        df.to_csv("C:/ViveCoding/KJH/옛날거/imdb_movies.csv", index=False, encoding='utf-8-sig')
        print("\nScraping complete. Data saved to 'KJH/옛날거/imdb_movies.csv'")
    else:
        print("\nNo data was scraped. Exiting.")

