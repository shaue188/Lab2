import os
import csv
import matplotlib.pyplot as plt

collision_dir = 'found-children/HashTable/collision'
time_dir = 'found-children/times'

output_dir = "plots2"

os.makedirs(output_dir, exist_ok=True)

# Парсим название файла, извлекаем размер
def extract_size(filename):
    return int(filename.split('_')[-1].replace('.csv', ''))

# Парсим файл с коллизиями
collision_data = []
for filename in os.listdir(collision_dir):
    if filename.endswith('.csv'):
        size = extract_size(filename)
        path = os.path.join(collision_dir, filename)
        with open(path, newline='', encoding='utf-8') as f:
            value = int(f.read().strip())
            collision_data.append((size, value))

collision_data.sort()

# Рисуем график коллизий
sizes = [x[0] for x in collision_data]
collisions = [x[1] for x in collision_data]
plt.figure(figsize=(10, 5))
plt.plot(sizes, collisions, marker='o')
plt.title('Зависимость количества коллизий от размера массива')
plt.xlabel('Размер массива')
plt.ylabel('Количество коллизий')
plt.grid(True)
plt.savefig(os.path.join(output_dir, 'plot-hash-collisions.png'))

# Структура: {method: [(size, time), ...]}
search_times = {}

for filename in os.listdir(time_dir):
    if filename.endswith('.csv'):
        size = extract_size(filename)
        path = os.path.join(time_dir, filename)
        with open(path, newline='', encoding='utf-8') as f:
            reader = csv.reader(f)
            for row in reader:
                if len(row) < 2:
                    continue
                method = row[0]
                time_ms = int(row[1])
                search_times.setdefault(method, []).append((size, time_ms))

# Сортируем точки графика по размеру для каждой из сортировок
for method in search_times:
    search_times[method].sort()

# Рисуем все поиски на 1 графике (обычном)
plt.figure(figsize=(10, 6))
for method, values in search_times.items():
    x = [size for size, _ in values]
    y = [time for _, time in values]
    plt.plot(x, y, marker='o', label=method)

plt.title('Время поиска (в мкс) по размеру массива')
plt.xlabel('Размер массива')
plt.ylabel('Время (мкс)')
plt.legend()
plt.grid(True)
plt.savefig(os.path.join(output_dir, 'plot-all.png'))

# Рисуем все поиски на 1 графике (логарифмическом)
plt.figure(figsize=(10, 6))
for method, values in search_times.items():
    x = [size for size, _ in values]
    y = [time for _, time in values]
    plt.plot(x, y, marker='o', label=method)

plt.title(f'Время поиска (в мкс) по размеру массива (логарифмическая шкала)')
plt.xlabel('Размер массива')
plt.ylabel("Время (мкс, лог)")
plt.yscale("log") # Логарифмическая шкала по y
plt.legend()
plt.grid(True)
plt.savefig(os.path.join(output_dir, 'plot-all-log.png'))

# Рисуем каждый поиск на отдельном графике (обычном)
for method, values in search_times.items():
    x = [size for size, _ in values]
    y = [time for _, time in values]
    plt.figure(figsize=(8, 4))
    plt.plot(x, y, marker='o')
    plt.title(f'Время поиска: {method}')
    plt.xlabel('Размер массива')
    plt.ylabel('Время (мкс)')
    plt.grid(True)
    plt.savefig(os.path.join(output_dir, f'plot-{method}.png'))

# Рисуем каждый поиск на отдельном графике (логарифмическом)
for method, values in search_times.items():
    x = [size for size, _ in values]
    y = [time for _, time in values]
    plt.figure(figsize=(8, 4))
    plt.plot(x, y, marker='o')
    plt.title(f'Время поиска: {method} (логарифмическая шкала)')
    plt.xlabel('Размер массива')
    plt.ylabel("Время (мкс, лог)")
    plt.yscale("log") # Логарифмическая шкала по y
    plt.grid(True)
    plt.savefig(os.path.join(output_dir, f'plot-{method}-log.png'))
