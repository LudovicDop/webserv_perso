import requests

with open("fichier_1G.bin", "rb") as f:
    r = requests.post("http://localhost:8000/upload", files={"file": f})
print(r.status_code)
