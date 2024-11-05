# multivariate output stacked lstm example
import numpy as np
from numpy import array
from numpy import hstack
from keras.models import Sequential
from keras.layers import LSTM
from keras.layers import Dense
from sklearn.metrics import mean_squared_error

# Split a multivariate sequence into samples
def split_sequences(sequences, n_steps):
    X, y = list(), list()
    for i in range(len(sequences)):
        end_ix = i + n_steps
        if end_ix > len(sequences):
            break
        seq_x, seq_y = sequences[i:end_ix, :-1], [sequences[end_ix-1, -1]]  # Wrap seq_y in a list
        X.append(seq_x)
        y.append(seq_y)
    return array(X), array(y)

# Khởi tạo một danh sách rỗng để lưu trữ các giá trị từ cột "sen1"
sen1_values = []
sen2_values = []
sen3_values = []
sen4_values = []
sen5_values = []
label_values =[]
# Mở tệp văn bản để đọc

with open('data.txt', 'r') as file:
	# Bỏ qua hàng đầu tiên (chứa tên cột)
	next(file)
    # Lặp qua từng dòng trong tệp
	for line in file:
	# Tách dòng thành các cột dựa trên khoảng trắng
		columns = line.strip().split()
		# Kiểm tra xem có ít nhất 2 cột (bao gồm "sen1")
		if len(columns) >= 2:
			sen1 = int(columns[1])
			sen2 = int(columns[2])
			sen3 = int(columns[3])
			sen4 = int(columns[4])
			sen5 = int(columns[5])
			label = int(columns[6])
			# Thêm giá trị vào danh sách sen1_values
			sen1_values.append(sen1)
			sen2_values.append(sen2)
			sen3_values.append(sen3)
			sen4_values.append(sen4)
			sen5_values.append(sen5)
			label_values.append(label)

# Bây giờ sen1_values chứa các giá trị từ cột "sen1"

sen1_values = array(sen1_values)
sen2_values = array(sen2_values)
sen3_values = array(sen3_values)
sen4_values = array(sen4_values)
sen5_values = array(sen5_values)
label_values = array(label_values)



# define input sequence


# in_seq1 = array([10, 20, 30, 40, 50, 60, 70, 80, 90])
# in_seq2 = array([15, 25, 35, 45, 55, 65, 75, 85, 95])
# out_seq = array([in_seq1[i]+in_seq2[i] for i in range(len(in_seq1))])
# # convert to [rows, columns] structure
# in_seq1 = in_seq1.reshape((len(in_seq1), 1))
# in_seq2 = in_seq2.reshape((len(in_seq2), 1))
# out_seq = out_seq.reshape((len(out_seq), 1))

# convert to [rows, columns] structure
train_sensor1_value = sen1_values.reshape((len(sen1_values), 1))
train_sensor2_value = sen2_values.reshape((len(sen2_values), 1))
train_sensor3_value = sen3_values.reshape((len(sen3_values), 1))
train_sensor4_value = sen4_values.reshape((len(sen4_values), 1))
train_sensor5_value = sen5_values.reshape((len(sen5_values), 1))
train_label_value = label_values.reshape((len(label_values), 1))

print(train_sensor1_value)
print(train_label_value)
# horizontally stack columns
dataset = hstack((train_sensor1_value, train_sensor2_value, train_sensor3_value, train_sensor4_value, train_sensor5_value, train_label_value ))
print('dataset is: ',dataset)
# choose a number of time steps
n_steps = 10
# convert into input/output
X, y = split_sequences(dataset, n_steps)
# the dataset knows the number of features, e.g. 2
# print(X)
# print(y)
n_features = X.shape[2]
print('features: ',n_features)
# define model
model = Sequential()
model.add(LSTM(100, activation='relu',input_shape=(n_steps, n_features)))
model.add(Dense(1))
model.compile(optimizer='adam', loss='mse')
# fit model
model.fit(X, y, epochs=500, verbose=1)
# demonstrate prediction
#x_input = array([[[10, 500, 500, 500, 500], [5, 500, 500, 500, 500], [0, 500, 500, 500, 500], [0, 500, 500, 500, 500], [0, 500, 500, 500, 500]]])
# # y_output = array([[60,65,125]])
# x_input = x_input.reshape((1, n_steps, n_features))
# yhat = model.predict(x_input, verbose=1)
# #accuracy = model.evaluate(x_input, y_output, batch_size=1)
# mse = mean_squared_error(y_output, yhat)
# print('Mean Squared Error:',1 - mse)
# print(yhat)

# x_input = array([[80, 85, 90, 95, 100], [85, 90, 95, 100, 105], [90, 95, 100, 105, 110], [95, 100, 105, 110, 115]])
# x_input = x_input.reshape((1, n_steps, n_features))
# yhat = model.predict(x_input, verbose=0)

#predict

while 1: 
	sen1_test_values = []
	sen2_test_values = []
	sen3_test_values = []
	sen4_test_values = []
	sen5_test_values = []
	file_name = input("filename: \n")
	with open(file_name, 'r') as file:
		# Bỏ qua hàng đầu tiên (chứa tên cột)
		#next(file)
		# Lặp qua từng dòng trong tệp
		for line in file:
		# Tách dòng thành các cột dựa trên khoảng trắng
			columns = line.strip().split()
			# Kiểm tra xem có ít nhất 2 cột (bao gồm "sen1")
			if len(columns) >= 2:
				sen1 = int(columns[1])
				sen2 = int(columns[2])
				sen3 = int(columns[3])
				sen4 = int(columns[4])
				sen5 = int(columns[5])
				#label = int(columns[6])
				# Thêm giá trị vào danh sách sen1_values
				sen1_test_values.append(sen1)
				sen2_test_values.append(sen2)
				sen3_test_values.append(sen3)
				sen4_test_values.append(sen4)
				sen5_test_values.append(sen5)
				#label_test_values.append(label)

	sen1_test_values = array(sen1_test_values)
	sen2_test_values = array(sen2_test_values)
	sen3_test_values = array(sen3_test_values)
	sen4_test_values = array(sen4_test_values)
	sen5_test_values = array(sen5_test_values)
	print(sen1_test_values)

	sen1_test_values = sen1_test_values.reshape((-1, 1))
	sen2_test_values = sen2_test_values.reshape((-1, 1))
	sen3_test_values = sen3_test_values.reshape((-1, 1))
	sen4_test_values = sen4_test_values.reshape((-1, 1))
	sen5_test_values = sen5_test_values.reshape((-1, 1))
	x_input = np.hstack((sen1_test_values, sen2_test_values, sen3_test_values, sen4_test_values, sen5_test_values))

	print(x_input)

	#x_input = array([sen1_test_values,sen2_test_values,sen3_test_values,sen4_test_values,sen5_test_values])
	x_input = x_input.reshape((1, n_steps, n_features))
	#print(x_input)
	yhat = model.predict(x_input, verbose=1)
	print(yhat)
