from random import randint
n = 500
e = (10**5)
minWeight = 0
maxWeight = 2000
# INF = 10**16
graph = [["INF" for _ in range(n)] for __ in range(n)]
for i in range(e):
    a = randint(0, n - 1)
    b = randint(0, n - 1)
    graph[a][b] = randint(minWeight, maxWeight)

cnt = 0
for i in range(n):
    for j in range(n):
        if i != j and graph[i][j] != "INF":
            cnt += 1

print(n, cnt)
for i in range(n):
    for j in range(n):
        if i != j and graph[i][j] != "INF":
            print(i, j, graph[i][j])