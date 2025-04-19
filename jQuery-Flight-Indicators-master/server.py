# filepath: c:\Users\OJTECH\Desktop\simulate\jQuery-Flight-Indicators-master\server.py
from flask import Flask, request, jsonify
from flask_cors import CORS
import mysql.connector

app = Flask(__name__)
CORS(app)  # Enable CORS for all routes

# MySQL Database Configuration
db_config = {
    'host': 'localhost',
    'user': 'root',  # Replace with your MySQL username
    'password': '',  # Replace with your MySQL password
    'database': 'flight_data'  # Replace with your database name
}

# Connect to the database
def get_db_connection():
    return mysql.connector.connect(**db_config)

# Create the table if it doesn't exist
def create_table():
    connection = get_db_connection()
    cursor = connection.cursor()
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS flight_logs (
            id INT AUTO_INCREMENT PRIMARY KEY,
            roll FLOAT,
            pitch FLOAT,
            heading FLOAT,
            altitude FLOAT,
            airspeed FLOAT,
            turn_coordinator FLOAT,
            timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
        )
    ''')
    connection.commit()
    cursor.close()
    connection.close()

create_table()

@app.route('/save-flightdata', methods=['POST'])
def save_flight_data():
    try:
        # Get the incoming flight data
        flight_data = request.json
        connection = get_db_connection()
        cursor = connection.cursor()

        # Insert the data into the database
        cursor.execute('''
            INSERT INTO flight_logs (roll, pitch, heading, altitude, airspeed, turn_coordinator)
            VALUES (%s, %s, %s, %s, %s, %s)
        ''', (
            flight_data.get('roll'),
            flight_data.get('pitch'),
            flight_data.get('heading'),
            flight_data.get('altitude'),
            flight_data.get('airspeed'),
            flight_data.get('turn_coordinator')
        ))

        connection.commit()
        cursor.close()
        connection.close()
        return jsonify({"message": "Flight data saved successfully"}), 200
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/get-flightlog', methods=['GET'])
def get_flight_log():
    try:
        connection = get_db_connection()
        cursor = connection.cursor(dictionary=True)

        # Retrieve all flight logs from the database
        cursor.execute('SELECT * FROM flight_logs ORDER BY timestamp DESC')
        logs = cursor.fetchall()

        cursor.close()
        connection.close()
        return jsonify(logs), 200
    except Exception as e:
        return jsonify({"error": str(e)}), 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)