rm result.test result.train  output.svm  model.svm result.csv
date > started.time
time ./DIP_DeepLearning  5 32 64 128 2 5 2 5 1807 < files.list
date > deep.time
svm-subset -s 0 result.csv 181 result.test result.train
date > subset.time
svm-train -s 0 -t 0 -c 1 result.train model.svm
date > train.time
svm-predict result.test model.svm output.svm
date > predict.time



rm result.test result.train  output.svm  model.svm 
svm-subset -s 1 result.csv 1300 result.test result.train
svm-train -s 0 -t 0 -c 1 result.train model.svm
svm-predict result.test model.svm output.svm 

