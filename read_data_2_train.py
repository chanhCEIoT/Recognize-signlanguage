import serial
import time
from queue import Queue
# this port address is for the serial tx/rx pins on the GPIO header

SERIAL_PORT = 'COM6'
# be sure to set this to the same rate used on the Arduino
label = 6
version = 1
SERIAL_RATE = 38400
DATA_FILE_NAME = './datatrain1/trainx_'+str(label)+'_v'+str(version)+'.txt'  # Specify the output file name
LABEL_FILE_NAME = './datatrain1/trainy_'+str(label)+'_v'+str(version)+'.txt'  # Specify the output file name
DATA_TEST_FILE_NAME = './datatrain1/testx_'+str(label)+'_v'+str(version)+'.txt'  # Specify the output file name
LABEL_TEST_FILE_NAME = './datatrain1/testy_'+str(label)+'_v'+str(version)+'.txt'  # Specify the output file name
test_counter = 300
queue_size = 240
windowSlide = 20
training_times = 200
testing_times = 20
def main():
    time.sleep(2)
    ser = serial.Serial(SERIAL_PORT, SERIAL_RATE,timeout=0.01)
    ser.reset_input_buffer()
    ser.reset_output_buffer()
    # Open the file for writing
    with open(DATA_FILE_NAME, "a") as data_file, open(LABEL_FILE_NAME, "a") as label_file, open(DATA_TEST_FILE_NAME, "a") as data_test_file,open(LABEL_TEST_FILE_NAME, "a") as label_test_file:
        print(f"Recording data to '{DATA_FILE_NAME}'. Press Ctrl+C to stop.")
        print(f"Recording data to '{LABEL_FILE_NAME}'. Press Ctrl+C to stop.")
        s0 = s1 = s2 = s3 = s4 = s5 = s6 = 0  # Initialize variables
        s0_queue = Queue()
        s1_queue = Queue()
        s2_queue = Queue()
        s3_queue = Queue()
        s4_queue = Queue() 
        s5_queue = Queue() 
        s6_queue = Queue() 
        i = 0
        test = 0
        j=0
        k=0
        try:
            while True:
                try:
                    reading = ser.readline().decode('utf-8')
                    #print(reading, end='')  # Print without newlines
                    reading = reading[:-1]
                    # Write the reading to the file
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
                            if test >= test_counter - queue_size + windowSlide:
                                s0_queue.put(int(values[0]))
                                s1_queue.put(int(values[1]))
                                s2_queue.put(int(values[2]))
                                s3_queue.put(int(values[3]))
                                s4_queue.put(int(values[4]))
                                s5_queue.put(int(values[5]))
                                s6_queue.put(int(values[6]))
                            test = test + 1
                        else:
                            s0_queue.put(int(values[0]))
                            s1_queue.put(int(values[1]))
                            s2_queue.put(int(values[2]))
                            s3_queue.put(int(values[3]))
                            s4_queue.put(int(values[4]))
                            s5_queue.put(int(values[5]))
                            s6_queue.put(int(values[6]))
                            print(f"Updated values: s0={s0}, s1={s1}, s2={s2}, s3={s3}, s4={s4}, s5={s5}, s6={s6}")
                            i = i + 1

                        if i >= windowSlide and test >= test_counter:
                            # do not dequeue
                            # for element in a_queue.queue: 
                            #     print(element)

                            # dequeue
                            # while not (s0_queue.empty() and s1_queue.empty() and s2_queue.empty() and s3_queue.empty() and s4_queue.empty()):
                            #     element = s0_queue.get()
                            #     print(element)
                            # dequeue
                            s0_list = list(s0_queue.queue)
                            s0_str = " ".join(map(str, s0_list))
                            s1_list = list(s1_queue.queue)
                            s1_str = " ".join(map(str, s1_list))
                            s2_list = list(s2_queue.queue)
                            s2_str = " ".join(map(str, s2_list))
                            s3_list = list(s3_queue.queue)
                            s3_str = " ".join(map(str, s3_list))
                            s4_list = list(s4_queue.queue)
                            s4_str = " ".join(map(str, s4_list))
                            s5_list = list(s5_queue.queue)
                            s5_str = " ".join(map(str, s5_list))
                            s6_list = list(s6_queue.queue)
                            s6_str = " ".join(map(str, s6_list))
                            combined_str = f"{s0_str} {s1_str} {s2_str} {s3_str} {s4_str} {s5_str} {s6_str}"
                            print(combined_str)
                            if j < training_times:
                                data_file.write(combined_str + '\n')
                                label_file.write(str(label) + '\n')
                                j=j+1
                            else:
                                if k < testing_times:
                                    data_test_file.write(combined_str + '\n')
                                    label_test_file.write(str(label) + '\n')
                                    k = k + 1
                                else:
                                    break
                            i = 0
                            # delete queue
                            # Calculate half size
                            print("real size of queue :" + str(s0_queue.qsize()))
                            # Dequeue half of the elements from each queue
                            for _ in range(windowSlide):
                                s0_queue.get()
                                s1_queue.get()
                                s2_queue.get()
                                s3_queue.get()
                                s4_queue.get()
                                s5_queue.get()
                                s6_queue.get()
                            # delete full queue
                            # while not (s0_queue.empty() and s1_queue.empty() and s2_queue.empty() and s3_queue.empty() and s4_queue.empty()):
                            #     s0_queue.get()
                            #     s1_queue.get()
                            #     s2_queue.get()
                            #     s3_queue.get()
                            #     s4_queue.get()
                except UnicodeDecodeError as e:
                    print(f"Error decoding serial data: {e}")
                    # Handle the error or simply continue to the next iteration
                    continue
        except KeyboardInterrupt:
            pass
        finally:
            ser.close()
            print("\nSerial connection closed.")

if __name__ == "__main__":
    main()
