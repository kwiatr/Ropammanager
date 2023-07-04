
ROPAMMANGER

This software connects to the ROPAM Alarm central and periodically reads it
status and gathers data from all the sensors connected to ROPAM central.
In addition it parses weather data received from Sainlogic FT0300 weather
station. It acts as REST server and is able to present all the data in form
of a single json.


Example json:
{
    "ALARM": {
        "Time": "23-07-04 04:36",
        "Firmware": "20",
        "Language": "p",
        "Inputs": "00000000000000000000000000000000",
        "Outputs": "xxxxxxxxxxxxxxxxxxxxxxxx0-------",
        "Armed": 0,
        "Siren": 0,
        "Zones": [
            {
                "Zone": 1,
                "Armed": 0,
                "NightArmed": 0,
                "OnDeparture": 0,
                "OnEntrance": 0,
                "IsALarm": 0,
                "IsTamper": 0,
                "IsReady": 1
            },
            {
                "Zone": 2,
                "Armed": 0,
                "NightArmed": 0,
                "OnDeparture": 0,
                "OnEntrance": 0,
                "IsALarm": 0,
                "IsTamper": 0,
                "IsReady": 1
            }
        ],
        "Temperature_in": 26.5,
        "Temperature_out": 25.5,
        "Voltage": 13.100000381469727
    },
    "WEATHER": {
        "tZewn": 16.699999999999999,
        "tIn": 25.699999999999999,
        "tStrych": 26.399999999999999,
        "tGaraz": 21.0,
        "tDol": 22.199999999999999,
        "tGBOX": 27.0,
        "wZewn": 75,
        "wIn": 45,
        "wStrych": 44,
        "wGaraz": 62,
        "wDol": 59,
        "wGBOX": 45,
        "wspd": 0.0,
        "wspdhi": 0.0,
        "wspdavg": 0.0,
        "rain": 0,
        "bar": 1005.9,
        "battery": "ok",
        "battery2": "ok",
        "battery5": "ok",
        "battery6": "ok",
        "date": "20230704",
        "time": "0436"
    }
}

This json can be consumed by any other monitoring system.
For telegraf/grafana I use the following configuration:
[[inputs.http]]
  urls = ["http://192.168.1.2:83/status.csv"]
  name_override = "alarm"
  timeout = "5s"
  tag_keys = ["Time"]
  data_format = "json_v2"
  [[inputs.http.json_v2]]
       [[inputs.http.json_v2.object]]
           path = "@this" 
