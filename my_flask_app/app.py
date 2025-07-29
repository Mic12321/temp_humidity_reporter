from flask import Flask, render_template, request, jsonify
from flask_socketio import SocketIO
from models import db, SensorData
import paho.mqtt.client as mqtt
import json
import os
from dotenv import load_dotenv
from datetime import datetime, timedelta



app = Flask(__name__)
load_dotenv()
mqtt_broker = os.getenv("MQTT_BROKER")
mqtt_port = int(os.getenv("MQTT_PORT", 1883))
mqtt_topic = os.getenv("MQTT_TOPIC", "test/topic")
app.config['SQLALCHEMY_DATABASE_URI'] = os.getenv("DATABASE_URL")
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False

db.init_app(app)

socketio = SocketIO(app)

# MQTT Setup
mqtt_client = mqtt.Client()

@app.route('/')
def latest():
    latest_data = SensorData.query.order_by(SensorData.timestamp.desc()).first()
    return render_template('latest.html', data=latest_data)

@app.route('/history')
def history():
    data = SensorData.query.order_by(SensorData.timestamp.desc()).limit(100).all()
    return render_template('history.html', data=data)

@app.route('/graphs')
def graphs():
    return render_template('graphs.html')

@app.route('/api/history')
def api_history():
    time_range = request.args.get('range', 'day')
    now = datetime.utcnow()

    if time_range == 'year':
        start_time = now - timedelta(days=365)
    elif time_range == 'week':
        start_time = now - timedelta(weeks=1)
    elif time_range == 'day':
        start_time = now - timedelta(days=1)
    elif time_range == 'hour':
        start_time = now - timedelta(hours=1)
    else:
        return jsonify({"error": "Invalid range"}), 400

    data = SensorData.query.filter(SensorData.timestamp >= start_time).order_by(SensorData.timestamp.asc()).all()

    result = [{
        "timestamp": d.timestamp.isoformat(),
        "temperature": d.temperature,
        "humidity": d.humidity
    } for d in data]

    return jsonify(result)


# MQTT Callbacks
def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT broker")
    client.subscribe(mqtt_topic)
    
def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
        

        if "error" in payload:
            error_msg = payload["error"]
            print(f"Sensor error: {error_msg}")
            socketio.emit('mqtt_error', {'error': error_msg})
            return

        # Proceed normally if no error
        temp = float(payload.get("temperature"))
        hum = float(payload.get("humidity"))

        # Save to database
        new_data = SensorData(temperature=temp, humidity=hum)
        with app.app_context():
            db.session.add(new_data)
            db.session.commit()

        print(f"Saved: {temp}°C, {hum}%")
        socketio.emit('mqtt_message', {'temperature': temp, 'humidity': hum})

    except Exception as e:
        print("Error processing MQTT message:", e)
        print("Payload:", msg.payload.decode())


mqtt_client.on_connect = on_connect
mqtt_client.on_message = on_message
mqtt_client.connect(mqtt_broker, mqtt_port, 60)
mqtt_client.loop_start()

if __name__ == '__main__':
    with app.app_context():
        db.create_all()
    socketio.run(app, debug=True)
