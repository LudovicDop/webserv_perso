#!/usr/bin/env python3
import cgi
import os

print("Content-Type: text/html\n")

form = cgi.FieldStorage()
fileitem = form['fichier']

if fileitem.filename:
    filename = os.path.basename(fileitem.filename)
    filepath = f"/tmp/{filename}"  # ou un autre dossier accessible
    with open(filepath, 'wb') as f:
        f.write(fileitem.file.read())
    print(f"<p>Fichier {filename} uploadé avec succès.</p>")
else:
    print("<p>Aucun fichier n'a été sélectionné.</p>")
