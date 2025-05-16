from flask import Flask, render_template_string

app = Flask(__name__)

@app.route('/')
def home():
    html_content = '''
    <!DOCTYPE html>
    <html>
    <head>
        <title>Ma Page Web en Python</title>
        <style>
            body {font-family: Arial, sans-serif; margin: 0; padding: 0; text-align: center;}
            header {background-color: #4CAF50; color: white; padding: 10px;}
            main {padding: 20px;}
            footer {background-color: #f1f1f1; padding: 10px; margin-top: 20px;}
        </style>
    </head>
    <body>
        <header>
            <h1>Bienvenue sur ma page web en Python</h1>
        </header>
        <main>
            <p>Ceci est une page web simple créée avec Flask en Python.</p>
            <a href="https://www.example.com">Visiter un lien</a>
        </main>
        <footer>
            <p>&copy; 2025 - Mon Site Web</p>
        </footer>
    </body>
    </html>
    '''
    return render_template_string(html_content)

if __name__ == '__main__':
    app.run(debug=True)
