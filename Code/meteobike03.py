#! /usr/bin/python
# -*- coding: utf-8 -*-
"""
Program <meteobike.py> to read and record GPS data, air temperature and humidity
using Adafruit's Ultimate GPS and Pimoroni's DHT22 temperature sensor while riding
on a bike.
University of Freiburg
Environmental Meteology
Version 1.2
Written by Heinz Christen Mar 2018
Modified by Andreas Christen Apr 2018
Using the class GpsPoller
written by Dan Mandle http://dan.mandle.me September 2012 License: GPL 2.0
Buttons:
Record:	Start recording in append mode to logfile, but only if gps has fix
Stop:	Stop recording (Pause)
Exit:	exit program
"""
import os
import sys
from gps import *
import adafruit_dht
import threading
import board
from tkinter import *
from time import gmtime, strftime
import numpy
import socket
import queue

q = queue.Queue()

def get_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # doesn't even have to be reachable
        s.connect(('10.255.255.255', 1))
        IP = s.getsockname()[0]
    except:
        IP = '127.0.0.1'
    finally:
        s.close()
    return IP


raspberryid = "00"  # enter your raspberry's number
studentname = "Unclaimed"  # enter your first name - no spaces and no special characters
# enter the calibration coefficient slope for temperature
temperature_cal_a1 = 1.0000
# enter the calibration coefficient offset for temperature
temperature_cal_a0 = 0.00000
vappress_cal_a1 = 1.00000  # enter the calibration coefficient slope for vapour pressure
# enter the calibration coefficient offset for vapour pressure
vappress_cal_a0 = 0.00000

window_title = "Meteobike"+raspberryid
logfile_path = "/home/strebdom/Desktop/"
logfile = logfile_path+raspberryid+"-"+studentname+"-" + \
    strftime("%Y-%m-%d-%H-%M-%S.csv")  # construct file name
font_size = 24
gpsd = None  # setting global variables
recording = False
sampling_rate = 5  # sampling rate - minimum number of seconds between samplings

e = threading.Event()

class GpsPoller(threading.Thread):
    def __init__(self):
        threading.Thread.__init__(self)
        global gpsd  # bring it in scope
        gpsd = gps(mode=WATCH_ENABLE)  # starting the stream of info
        self.current_value = None
        self.running = True  # setting the thread running to true

    def run(self):
        global gpsd
        while gpsp.running:
            if e.is_set():
                break
            # this will continue to loop and grab EACH set of gpsd info to clear the buffer
            gpsd.next()


# main program
#counter = 0
gpsp = GpsPoller()  # create the thread
gpsp.start()  # start it up
dht22_pin = 4  # pin for DHT22 Data
dht22_sensor = adafruit_dht.DHT22(board.D4)
# callback functions


def exit_program():
    e.set()
    th.join()
    master.destroy()


def record_data():
    global recording
    recording = True
    b2.config(state=NORMAL)
    b1.config(state=DISABLED)
    if os.path.isfile(logfile):
        return
    else:  # write header line
        f0 = open(logfile, "w")
        f0.write("ID,Record,Raspberry_Time,GPS_Time,Altitude,Latitude,Longitude,Temperature,TemperatureRaw,RelHumidity,RelHumidityRaw,VapourPressure,VapourPressureRaw\n")
        f0.close()


def stop_data():
    global recording
    recording = False
    b1.config(state=NORMAL)
    b2.config(state=DISABLED)


def start_counting(e):
    counter= 1
    while 1:
        if e.is_set():
            sys.exit()
        try:
           res_dic=measure_loop(counter)
           counter+=1
           q.put(res_dic)
           master.event_generate("<<event1>>", when="tail")
        except:
           continue
        time.sleep(5)
        print(counter)

def measure_loop(counter):
    computer_time = strftime("%Y-%m-%d %H:%M:%S")
    dht22_humidity, dht22_temperature = dht22_sensor.humidity, dht22_sensor.temperature
    dht22_temperature_raw = round(dht22_temperature, 5)
    dht22_temperature_calib = round(
        dht22_temperature * temperature_cal_a1 + temperature_cal_a0, 3)
    dht22_temperature = dht22_temperature_calib
    saturation_vappress_ucalib = 0.6113 * \
        numpy.exp((2501000.0/461.5)*((1.0/273.15) -
                    (1.0/(dht22_temperature_raw+273.15))))
    saturation_vappress_calib = 0.6113 * \
        numpy.exp((2501000.0/461.5)*((1.0/273.15) -
                    (1.0/(dht22_temperature_calib+273.15))))
    dht22_vappress = (dht22_humidity/100.0)*saturation_vappress_ucalib
    dht22_vappress_raw = round(dht22_vappress, 3)
    dht22_vappress_calib = round(
        dht22_vappress * vappress_cal_a1 + vappress_cal_a0, 3)
    dht22_vappress = dht22_vappress_calib
    dht22_humidity_raw = round(dht22_humidity, 5)
    dht22_humidity = round(
        100 * (dht22_vappress_calib / saturation_vappress_calib), 5)
    if dht22_humidity > 100:
        dht22_humidity = 100
    gps_time = gpsd.utc
    gps_altitude = gpsd.fix.altitude
    gps_latitude = gpsd.fix.latitude
    gps_longitude = gpsd.fix.longitude
    f_mode = int(gpsd.fix.mode)  # store number of sats
    has_fix = False  # assume no fix
    # if f_mode == 2:
    #     value_counter.config(bg="orange")
    # elif f_mode > 2:
    #     has_fix = True
    #     value_counter.config(bg="#20ff20")  # light green
    # else:
    #     value_counter.config(bg="red")
    # value_ctime.config(text=computer_time)
    # value_altitude.config(text="{0:.3f} m".format(gps_altitude))
    # value_latitude.config(text="{0:.6f} N".format(gps_latitude))
    # value_longitude.config(text="{0:.6f} E".format(gps_longitude))
    # value_time.config(text=gps_time)  # cut last 5 letters
    # value_temperature.config(text="{0:.1f}ºC".format(dht22_temperature))
    # value_humidity.config(text="{0:.1f} %".format(dht22_humidity))
    # value_vappress.config(text="{0:.3f} kPa".format(dht22_vappress))
    # label.config(text=str(counter))
    res_dic = {}
    if f_mode == 2:
        res_dic["bg"] = "orange"
    elif f_mode > 2:
        has_fix = True
        res_dic["bg"] = "#20ff20"
    else:
        res_dic["bg"] = "red"
    res_dic["value_ctime_config"] = computer_time
    res_dic["value_altitude_config"] = "{0:.3f} m".format(gps_altitude)
    res_dic["value_latitude_config"] = "{0:.6f} N".format(gps_latitude)
    res_dic["value_longitude_config"] = "{0:.6f} E".format(gps_longitude)
    res_dic["value_time_config"] = gps_time
    res_dic["value_temperature_config"] = "{0:.1f}ºC".format(dht22_temperature)
    res_dic["value_humidity_config"] = "{0:.1f} %".format(dht22_humidity)
    res_dic["value_vappress_config"] = "{0:.3f} kPa".format(dht22_vappress)
    res_dic["counter"] = counter
    if recording and has_fix:
        f0 = open(logfile, "a")
        f0.write(raspberryid+",")
        f0.write(str(counter)+",")
        f0.write(computer_time+",")
        if has_fix:
            f0.write(gps_time+",")
        else:
            f0.write("nan,")
        f0.write("{0:.3f}".format(gps_altitude)+",")
        f0.write("{0:.6f}".format(gps_latitude)+",")
        f0.write("{0:.6f}".format(gps_longitude)+",")
        f0.write(str(dht22_temperature)+",")
        f0.write(str(dht22_temperature_raw)+",")
        f0.write(str(dht22_humidity)+",")
        f0.write(str(dht22_humidity_raw)+",")
        f0.write(str(dht22_vappress)+",")
        f0.write(str(dht22_vappress_raw)+"\n")
        f0.close()
    return res_dic


def eventhandler(evt):
    res_dic = q.get()
    value_ctime.config(text=res_dic["value_ctime_config"])
    value_altitude.config(text=res_dic["value_altitude_config"])
    value_latitude.config(text=res_dic["value_latitude_config"])
    value_longitude.config(text=res_dic["value_longitude_config"])
    value_time.config(text=res_dic["value_time_config"])  # cut last 5 letters
    value_temperature.config(text=res_dic["value_temperature_config"])
    value_humidity.config(text=res_dic["value_humidity_config"])
    value_vappress.config(text=res_dic["value_vappress_config"])
    value_counter.config(text=str(res_dic["counter"]), bg=res_dic["bg"])
    
   

# define widgets
master = Tk()
master.title(window_title)
#master.attributes('-fullscreen', True)
name1 = Label(master, text=" Name", fg="blue", font=(
    'Helvetica', font_size)).grid(row=0, column=0, sticky=W)
name2 = Label(master, text=studentname+"'s Meteobike", fg="blue",
              font=('Helvetica', font_size)).grid(row=0, column=1, sticky=W, columnspan=2)
ip1 = Label(master, text=" IP", fg="blue", font=(
    'Helvetica', font_size)).grid(row=1, column=0, sticky=W)
ip2 = Label(master, text=get_ip(), fg="blue", font=(
    'Helvetica', font_size)).grid(row=1, column=1, sticky=W, columnspan=2)
# define labels
label_counter = Label(master, text=" Counter", font=('Helvetica', font_size))
label_counter.grid(row=2, column=0, sticky=W)
label_ctime = Label(master, text=" Time", font=('Helvetica', font_size))
label_ctime.grid(row=3, column=0, sticky=W)
label_altitude = Label(master, text=" Altitude", font=('Helvetica', font_size))
label_altitude.grid(row=4, column=0, sticky=W)
label_latitude = Label(master, text=" Latitude", font=('Helvetica', font_size))
label_latitude.grid(row=5, column=0, sticky=W)
label_longitude = Label(master, text=" Longitude",
                        font=('Helvetica', font_size))
label_longitude.grid(row=6, column=0, sticky=W)
label_time = Label(master, text=" GPS Time", font=('Helvetica', font_size))
label_time.grid(row=7, column=0, sticky=W)
label_temperature = Label(master, text=" Temperature",
                          font=('Helvetica', font_size))
label_temperature.grid(row=8, column=0, sticky=W)
label_humidity = Label(master, text=" Rel. Humidity",
                       font=('Helvetica', font_size))
label_humidity.grid(row=9, column=0, sticky=W)
label_vappress = Label(master, text=" Vap. Pressure   ",
                       font=('Helvetica', font_size))
label_vappress.grid(row=10, column=0, sticky=W)
# define values (constructed also as labels, text will be modified in count)
value_counter = Label(master, text=" Counter", bg="red",
                      font=('Helvetica', font_size))
value_counter.grid(row=2, column=1, sticky=W, columnspan=2)
value_ctime = Label(master, text=" Time", font=('Helvetica', font_size))
value_ctime.grid(row=3, column=1, sticky=W, columnspan=2)
value_altitude = Label(master, text=" Altitude", font=('Helvetica', font_size))
value_altitude.grid(row=4, column=1, sticky=W, columnspan=2)
value_latitude = Label(master, text=" Latitude", font=('Helvetica', font_size))
value_latitude.grid(row=5, column=1, sticky=W, columnspan=2)
value_longitude = Label(master, text=" Longitude",
                        font=('Helvetica', font_size))
value_longitude.grid(row=6, column=1, sticky=W, columnspan=2)
value_time = Label(master, text=" GPS Time ---------------",
                   font=('Helvetica', font_size))
value_time.grid(row=7, column=1, sticky=W, columnspan=2)
value_temperature = Label(master, text=" Temperature",
                          font=('Helvetica', font_size))
value_temperature.grid(row=8, column=1, sticky=W, columnspan=2)
value_humidity = Label(master, text=" Rel. Humidity",
                       font=('Helvetica', font_size))
value_humidity.grid(row=9, column=1, sticky=W, columnspan=2)
value_vappress = Label(master, text=" Vap. Pressure ",
                       font=('Helvetica', font_size))
value_vappress.grid(row=10, column=1, sticky=W, columnspan=2)
# initialize value_counter
th = threading.Thread(target=start_counting, args=(e,))
th.daemon=True
#start_counting(value_counter)
# define buttons
b1 = Button(master, text='Record', width=7,
            state=DISABLED, command=record_data)
b1.grid(row=12, column=0, sticky=W)
b2 = Button(master, text='Stop', width=7, state=DISABLED, command=stop_data)
b2.grid(row=12, column=1, sticky=W)
b4 = Button(master, text='Exit', width=7, state=NORMAL, command=exit_program)
b4.grid(row=12, column=2, sticky=E)
recording = True
record_data()
#wait in mainloop
th.start()
master.bind("<<event1>>", eventhandler)
mainloop()
