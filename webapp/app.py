import serial
import dash
from dash.dependencies import Output, Input
import dash_core_components as dcc
import dash_html_components as html
import plotly
import plotly.graph_objs as go

ser = serial.Serial("COM4", 115200)  # Change COM3 to your port name

NUM_SAMPLES = 100
x_data = [i for i in range(NUM_SAMPLES)]
temp_data = [0] * NUM_SAMPLES
hum_data = [0] * NUM_SAMPLES

app = dash.Dash(__name__)
app.layout = html.Div(
    html.Div(
        [
            dcc.Graph(id="live-update-graph-scatter", animate=True),
            dcc.Interval(id="interval-component", interval=1 * 1000, n_intervals=0),
        ]
    )
)


@app.callback(
    Output("live-update-graph-scatter", "figure"),
    Input("interval-component", "n_intervals"),
)
def update_graph_scatter(n):
    global x_data
    global y_data

    # Clean the serial port
    ser.reset_input_buffer()

    # Send a command to the Arduino to read the sensor
    ser.write(b"a\n")
    # Read the data from the serial port
    data = ser.readline().decode().strip()
    # Split the data by comma
    temp, hum = data.split(", ")
    # Convert the data to float
    temp = float(temp[2:])
    hum = float(hum[2:])

    x_data.append(x_data[-1] + 1)
    x_data.pop(0)
    temp_data.append(temp)
    temp_data.pop(0)
    hum_data.append(hum)
    hum_data.pop(0)

    temp_plot = plotly.graph_objs.Scatter(
        x=x_data,
        y=temp_data,
        name="Temperature",
        mode="lines+markers",
        line=dict(color="red"),
    )

    hum_plot = plotly.graph_objs.Scatter(
        x=x_data,
        y=hum_data,
        name="Humidity",
        mode="lines+markers",
        line=dict(color="blue"),
    )

    return {
        "data": [temp_plot, hum_plot],
        "layout": go.Layout(
            xaxis=dict(range=[min(x_data), max(x_data)], title="Samples"),
            yaxis=dict(range=[0, 100], title="Value"),
            title="DHT11",
        ),
    }


if __name__ == "__main__":
    app.run_server(debug=False)
