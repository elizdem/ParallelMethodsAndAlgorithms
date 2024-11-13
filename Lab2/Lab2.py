import os
import time
import zipfile
import requests
from bs4 import BeautifulSoup
from mpi4py import MPI
import aiohttp
import asyncio
from urllib.parse import urljoin

BASE_URL = "http://example.com"

# Функция для скачивания данных с URL
def download_url(url):
    print(f"Start downloading: {url}")
    response = requests.get(url)
    print(f"Download complete: {url}")
    return response.text

# Функция для парсинга HTML-страницы и поиска изображений
def parse_for_images(html):
    soup = BeautifulSoup(html, 'html.parser')
    images = []
    for img in soup.find_all('img'):
        src = img.get('src')
        if src:
            images.append(src)
    return images

# Асинхронная функция для загрузки изображения
async def download_image(session, image_url):
    full_image_url = urljoin(BASE_URL, image_url)  
    print(f"Downloading image: {full_image_url}")
    async with session.get(full_image_url) as response:
        image_data = await response.read()
    return image_data

# Функция для добавления изображения в архив
def add_to_zip(zip_filename, image_name, image_data):
    with zipfile.ZipFile(zip_filename, 'a') as zipf:
        zipf.writestr(image_name, image_data)

# Асинхронная функция для скачивания изображений (для одного процесса)
async def download_images(images):
    async with aiohttp.ClientSession() as session:
        tasks = [download_image(session, img) for img in images]
        return await asyncio.gather(*tasks)

# Основная функция для обработки загрузки URL-адресов с использованием MPI
def main():
    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()
    size = comm.Get_size()

    # Чтение списка URL из файла
    url_file = "urls.txt"
    with open(url_file, 'r') as file:
        urls = [line.strip() for line in file.readlines()]

    # Разделяем URL между процессами
    chunk_size = len(urls) // size
    start = rank * chunk_size
    end = (rank + 1) * chunk_size if rank != size - 1 else len(urls)
    local_urls = urls[start:end]

    # Загружаем страницы
    html_responses = [download_url(url) for url in local_urls]

    # Парсим страницы на наличие изображений
    all_images = []
    for html in html_responses:
        images = parse_for_images(html)
        all_images.extend(images)

    # Синхронизация процесса MPI
    comm.Barrier()

    # Запускаем асинхронную загрузку изображений в процессе с rank 0
    image_data_list = []
    if rank == 0:
        start_time = time.time()
        loop = asyncio.get_event_loop()  # Получаем текущий цикл событий
        image_data_list = loop.run_until_complete(download_images(all_images))  # Запускаем асинхронную задачу
        end_time = time.time()

    # Добавляем изображения в архив на всех процессах
    for idx, image_data in enumerate(image_data_list):
        image_name = f"image_{rank}_{idx}.jpg"
        add_to_zip("images.zip", image_name, image_data)

    # Синхронизируем процессы
    comm.Barrier()

if __name__ == "__main__":
    start_time = time.time()  # Время начала работы программы
    main()
    end_time = time.time()  # Время завершения работы программы
    print(f"Total execution time: {end_time - start_time:.2f} seconds")
