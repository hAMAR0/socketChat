from flask import Flask
from flask import render_template

app = Flask(__name__)
with open("./chat", 'r', encoding="utf-8") as f:
    chat = f.read()
@app.route('/')
def hello_world():
    return render_template("index.html", text = chat)

if __name__ == "__main__":
    app.run(debug=True)