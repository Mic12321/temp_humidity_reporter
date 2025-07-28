from flask import Flask, render_template
from flask_socketio import SocketIO
from models import db, SensorData
import paho.mqtt.client as mqtt
import json

app = Flask(__name__)
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///mqtt_data.db'
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
    # For now just a placeholder page
    return render_template('history.html')

@app.route('/graphs')
def graphs():
    # Placeholder page for graphs or future features
    return render_template('graphs.html')

# MQTT Callbacks
def on_connect(client, userdata, flags, rc):
    print("Connected to MQTT broker")
    client.subscribe("test/topic")
    
def on_message(client, userdata, msg):
    try:
        payload = json.loads(msg.payload.decode())
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
mqtt_client.connect("192.168.7.80", 18830, 60)
mqtt_client.loop_start()

if __name__ == '__main__':
    with app.app_context():
        db.create_all()
    socketio.run(app, debug=True)
