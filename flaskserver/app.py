from flask import Flask, request

app= Flask(__name__)

@app.route('/upload', methods=['POST'])
def upload():
    data = request.data
    print("Received data: ", data)
    return 'Data received', 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)