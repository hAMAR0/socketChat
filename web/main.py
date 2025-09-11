from flask import Flask
from flask import render_template, jsonify

app = Flask(__name__)

@app.route('/')
def index():
    return render_template("index.html", text = "Loading...")

@app.route('/get_chat')
def get_chat():
    with open("./chat", 'r', encoding="utf-8") as f:
        chat = f.read()
    return jsonify({"text": chat})


if __name__ == "__main__":
    app.run(debug=True)