from flask import Flask, request, jsonify
from pymongo import MongoClient
import datetime

app = Flask(__name__)

client = MongoClient("localhost", 27017)
db = client['C3_Mini']               # Generic database name
collection = db['sensor_data']       # Stores  sensor data
collection_sgp30 = db['sgp30']       # Stores  sensor data
collection_hp303b = db['hp303b']       # Stores  sensor data

print(client, db, collection)

# ------------------------------------------
#  POST: Add new sensor readings
# ------------------------------------------
@app.route('/api/sensor', methods=['POST'])
def write_sensor_data():
    try:
        data = request.get_json()
        print(f"Received Data: {data}")

        # Attach timestamp automatically
        timestamp = datetime.datetime.now().isoformat()
        data["timestamp"] = timestamp

        # Insert into MongoDB
        result = collection.insert_one(data)

        return jsonify({
            'message': 'Data successfully inserted.',
            'document_id': str(result.inserted_id),
            'timestamp': timestamp
        }), 201

    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/api/sgp30', methods=['POST'])
def write_sgp30_sensor_data():
    try:
        data = request.get_json()
        print(f"Received Data: {data}")

        # Attach timestamp automatically
        timestamp = datetime.datetime.now().isoformat()
        data["timestamp"] = timestamp

        # Insert into MongoDB
        result = collection_sgp30.insert_one(data)

        return jsonify({
            'message': 'Data successfully inserted.',
            'document_id': str(result.inserted_id),
            'timestamp': timestamp
        }), 201

    except Exception as e:
        return jsonify({'error': str(e)}), 500


@app.route('/api/hp303b', methods=['POST'])
def write_hp303b_data():
    try:
        data = request.get_json()
        print(f"Received Data: {data}")

        # Attach timestamp automatically
        timestamp = datetime.datetime.now().isoformat()
        data["timestamp"] = timestamp

        # Insert into MongoDB
        result = collection_hp303b.insert_one(data)

        return jsonify({
            'message': 'Data successfully inserted.',
            'document_id': str(result.inserted_id),
            'timestamp': timestamp
        }), 201

    except Exception as e:
        return jsonify({'error': str(e)}), 500


# ------------------------------------------
#  GET: Retrieve all data
# ------------------------------------------
@app.route('/api/sensor', methods=['GET'])
def get_sensor_data():
    try:
        sensor_documents = list(collection.find({}, {'_id': 0}))

        if not sensor_documents:
            return jsonify({'message': 'No data found'}), 404

        return jsonify(sensor_documents), 200

    except Exception as e:
        return jsonify({'error': str(e)}), 500


# ------------------------------------------
#  Run Server
# ------------------------------------------
if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)
