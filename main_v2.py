# cnn lstm model
from queue import Queue
from numpy import mean
from numpy import std
from numpy import dstack
from numpy import array, dstack
import numpy as np
import tensorflow as tf
from keras.models import Sequential
from keras.layers import Dense
from keras.layers import Flatten
from keras.layers import Dropout
from keras.layers import SpatialDropout1D
from keras.layers import LSTM
from keras.layers import TimeDistributed
from keras.layers import Conv1D
from keras.layers import MaxPooling1D
from keras.utils import to_categorical
from keras.models import load_model
from matplotlib import pyplot
import pyttsx3
import serial
import time
from sklearn.preprocessing import MinMaxScaler
# import pygame
import sys
#pygame.init()
engine = pyttsx3.init() # object creation
SERIAL_PORT = 'COM6'
# be sure to set this to the same rate used on the Arduino
SERIAL_RATE = 38400
test_counter = 240 # test_couter > queue size
queue_size = 240
timeReconnect = 20 
windowSlide = 120
trainx_file = "./datatrain/trainx.txt"
trainy_file = "./datatrain/trainy.txt"
testx_file = "./datatrain/testx.txt"
testy_file = "./datatrain/testy.txt"
config_file = "./datatrain/config.txt"

# screen_width = 800
# screen_height = 600
# screen = pygame.display.set_mode((screen_width, screen_height))
# pygame.display.set_caption("Real-time Display")

# Set up fonts
#font = pygame.font.Font(None, 36)
#WHITE = (255, 255, 255)
""" RATE"""
#rate = engine.getProperty('rate')   # getting details of current speaking rate
engine.setProperty('rate', 125)     # setting up new voice rate
"""VOLUME"""
#volume = engine.getProperty('volume')   #getting to know current volume level (min=0 and max=1)
#print (volume)                          #printing current volume level
engine.setProperty('volume',1.0)    # setting up volume level  between 0 and 1

"""VOICE"""
#voices = engine.getProperty('voices')       #getting details of current voice
#engine.setProperty('voice', voices[0].id)  #changing index, changes voices. o for male
engine.setProperty('voice', engine.getProperty('voices')[1].id)   #changing index, changes voices. 1 for female

def textToSpeech(text):
    engine.say(text)
    engine.runAndWait()
    engine.stop()
    

def readConfig():
    with open('./datatrain/config.txt', 'r',encoding='utf-8') as file:
        # Initialize an empty 2D array
        config = []

        # Iterate over each line in the file
        for line in file:
            # Split the line into individual words
            words = line.strip()

            # Append the words to the 2D array
            config.append(words)
        return config

def readdata():
    trainx_data =[]
    trainy_data =[]
    testx_data =[]
    testy_data =[]
    with open(trainx_file, 'r') as trainx, open(trainy_file, 'r') as trainy,open(testx_file, 'r') as testx, open(testy_file, 'r') as testy  :
        ytrain = []
        ytest = []
        for line in trainy:
            ytrain.append(line)
        ytrain= array(ytrain)
        trainy_data = np.vstack(ytrain)
        trainy_data.reshape(1,len(ytrain))
        print(trainy_data.shape)
        for line in testy:
            ytest.append(line)
        ytest= array(ytest)
        testy_data = np.vstack(ytest)
        testy_data.reshape(1,len(ytest))
        print(testy_data.shape)
        
        while True:
            try:
                x1 = []
                x2 = []
                x3 = []
                x4 = []
                x5 = []
                x6 = []
                x7 = []
                line = next(trainx).strip().split()
                if len(line) >= 1680:
                    for i in range(240):
                        x1.append(int(line[i]))
                        x2.append(int(line[i + 240]))
                        x3.append(int(line[i + 480]))
                        x4.append(int(line[i + 720]))
                        x5.append(int(line[i + 960]))
                        x6.append(int(line[i + 1200]))
                        x7.append(int(line[i + 1440]))
                else:
                    print("Error train data.")
                x1 = array(x1)
                x2 = array(x2)
                x3 = array(x3)
                x4 = array(x4)
                x5 = array(x5)
                x6 = array(x6)
                x7 = array(x7)
                line_dataset = dstack([x1, x2, x3, x4, x5, x6, x7]) 
                line_dataset = line_dataset.reshape(1,240,7)
                trainx_data.append(line_dataset)
            except StopIteration:
                break
        while True:
            try:
                x1 = []
                x2 = []
                x3 = []
                x4 = []
                x5 = []
                x6 = []
                x7 = []
                line = next(testx).strip().split()
                if len(line) >= 1680:
                    for i in range(240):
                        x1.append(int(line[i]))
                        x2.append(int(line[i + 240]))
                        x3.append(int(line[i + 480]))
                        x4.append(int(line[i + 720]))
                        x5.append(int(line[i + 960]))
                        x6.append(int(line[i + 1200]))
                        x7.append(int(line[i + 1440]))
                else:
                    print("Error test data.")
                x1 = array(x1)
                x2 = array(x2)
                x3 = array(x3)
                x4 = array(x4)
                x5 = array(x5)
                x6 = array(x6)
                x7 = array(x7)
                line_dataset = dstack([x1, x2, x3, x4, x5, x6, x7]) 
                line_dataset = line_dataset.reshape(1,240,7)
                testx_data.append(line_dataset)
            except StopIteration:
                break	
    trainx_data = np.vstack(trainx_data)
    testx_data = np.vstack(testx_data)
    trainy_data = to_categorical(trainy_data)
    testy_data = to_categorical(testy_data)
    return trainx_data, trainy_data, testx_data, testy_data

def predict_from_uart(ser,model): # this function is used to detect activity
    time.sleep(1)
    s0 = s1 = s2 = s3 = s4 = s5 = s6 = 0  # Initialize variables
    s0_queue = []
    s1_queue = []
    s2_queue = []
    s3_queue = []
    s4_queue = []
    s5_queue = []
    s6_queue = []
    i = 0
    j = 0
    z = 0
    test = 0
    checkEr = -1
    checkErCount = 0
    check_recog = 0
    try:
        while True:
            try:
                reading = ser.readline().decode('utf-8')
                if reading:
                    reading = reading[:-1]
                    values = reading.split()
                    if len(values) == 7:
                        s0 = int(values[0])
                        s1 = int(values[1])
                        s2 = int(values[2])
                        s3 = int(values[3])
                        s4 = int(values[4])   
                        s5 = int(values[5])
                        s6 = int(values[6])  
                        if test < test_counter:
                            print(f"Updated values in test time: s0={s0}, s1={s1}, s2={s2}, s3={s3}, s4={s4}, s5={s5}, s6={s6}")
                            # after testing time, add value to queue - windowSlide
                            test = test + 1
                            if test > test_counter - queue_size + windowSlide:
                                s0_queue.append(int(values[0]))
                                s1_queue.append(int(values[1]))
                                s2_queue.append(int(values[2]))
                                s3_queue.append(int(values[3]))
                                s4_queue.append(int(values[4]))
                                s5_queue.append(int(values[5]))
                                s6_queue.append(int(values[6]))
                        #add data to full queue
                        else:
                            s0_queue.append(int(values[0]))
                            s1_queue.append(int(values[1]))
                            s2_queue.append(int(values[2]))
                            s3_queue.append(int(values[3]))
                            s4_queue.append(int(values[4]))
                            s5_queue.append(int(values[5]))
                            s6_queue.append(int(values[6]))
                            i = i + 1
                        if check_recog and z < queue_size - windowSlide:
                            s0_queue.append(int(values[0]))
                            s1_queue.append(int(values[1]))
                            s2_queue.append(int(values[2]))
                            s3_queue.append(int(values[3]))
                            s4_queue.append(int(values[4]))
                            s5_queue.append(int(values[5]))
                            s6_queue.append(int(values[6]))
                            z = z + 1
                            i = 0
                        else:
                            if i >= windowSlide and test >= test_counter:
                                s0_array = np.array(s0_queue)
                                s1_array = np.array(s1_queue)
                                s2_array = np.array(s2_queue)
                                s3_array = np.array(s3_queue)
                                s4_array = np.array(s4_queue)
                                s5_array = np.array(s5_queue)
                                s6_array = np.array(s6_queue)
                                combined_matrix = np.dstack((s0_array, s1_array, s2_array, s3_array, s4_array, s5_array, s6_array))
                                #predict activity here
                                
                                yhat = model.predict(combined_matrix, verbose = 0)
                                result = yhat
                                rounded_result = np.round(result, 2) 
                                # print('raw result is  ', rounded_result)
                                #merge it with config:
                                config = readConfig()
                                if len(rounded_result[0]) == len(config):
                                    index = np.where(rounded_result > 0.85)
                                    # print(index[1][0])
                                    
                                    if len(index[0]) == 0:
                                        print(" ")   
                                        check_recog = 0
                                    # elif checkEr == [index[1][0]] and checkErCount == 0:
                                    #     if checkErCount == 0:
                                    #         checkErCount = 1
                                    #     print(" ")
                                    #     check_recog = 1
                                    else:
                                        print(config[index[1][0]])
                                        check_recog = 1
                                        #textToSpeech(config[index[1][0]])
                                        # checkEr = [index[1][0]]
                                        # checkErCount = 0
                                else:
                                    print("Error config file")
                                if check_recog == 1:
                                    for _ in range(queue_size):
                                        s0_queue.pop(0)
                                        s1_queue.pop(0)
                                        s2_queue.pop(0)
                                        s3_queue.pop(0)
                                        s4_queue.pop(0)
                                        s5_queue.pop(0)
                                        s6_queue.pop(0)
                                        z = 0
                                else:
                                    for _ in range(windowSlide):
                                        s0_queue.pop(0)
                                        s1_queue.pop(0)
                                        s2_queue.pop(0)
                                        s3_queue.pop(0)
                                        s4_queue.pop(0)
                                        s5_queue.pop(0)
                                        s6_queue.pop(0)
                                        i = 0
                                        j = 0
                else:
                    print("bad connection")
                    j = j + 1
                    if j > timeReconnect:
                        print("Reconnect....") 
                        ser.close()
                        print("\nSerial connection closed.")
                        break
            except UnicodeDecodeError as e:
                print(f"Error decoding serial data: {e}")
                continue
    except KeyboardInterrupt:
        pass

def evaluate_model(trainX, trainy, testX, testy,r = 0):
    # define model
    path = './model/train'+str(r)+'.h5'
    verbose, epochs, batch_size = 2, 300, 4000
    n_timesteps, n_features, n_outputs = trainX.shape[1], trainX.shape[2], trainy.shape[1]
    print("time steps = ", trainX.shape[1])
    #define model
    model = Sequential()
    model.add(LSTM(units = 128, input_shape = (n_timesteps, n_features)))
    model.add(Dropout(0.5)) 
    model.add(Flatten())
    model.add(Dense(units = 64, activation='relu'))
    model.add(Dense(n_outputs, activation='softmax'))
    model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])
    model.fit(trainX, trainy, epochs=epochs, batch_size=batch_size, verbose=verbose)
    model.save(path)
    # evaluate model
    _, accuracy = model.evaluate(testX, testy, batch_size=batch_size, verbose=2)
    return model,accuracy
# summarize scores
def summarize_results(scores):
    print(scores)
    m, s = mean(scores), std(scores)
    print('Accuracy: %.3f%% (+/-%.3f)' % (m, s))

# run an experiment
def run_experiment():
    # load data
    # repeat experiment
    
    while True:
        mode = int(input(" input 1 to load model, input 2 to re-train model base on text file: "))
        if mode == 1:
            model = tf.keras.models.load_model('./model/train0.h5')
            break
        if mode == 2:
            trainX, trainy, testX, testy = readdata()
            scores = list()
            repeats= int(input(" input number of test: "))
            for r in range(repeats):
                model, score = evaluate_model(trainX, trainy, testX, testy, r)
                score = score * 100.0
                print('>#%d: %.3f' % (r+1, score))
                score = np.round(score, 3) 
                scores.append(score)
            # summarize results
            summarize_results(scores)
            mode = int(input(" press anything to continute"))
            break
        else:
            print(" press anything to continute")
    while 1:    
        ser = None
        while ser is None:
            try:
                ser = serial.Serial(SERIAL_PORT, SERIAL_RATE,timeout=0.1)
                ser.reset_input_buffer()
                ser.reset_output_buffer()
                predict_from_uart(ser,model)
            except serial.SerialException as e:
                print(f"Failed to open serial port: {e}")
                print("Retrying in 1 second...")
                time.sleep(1)
                continue

# run the experiment
run_experiment()