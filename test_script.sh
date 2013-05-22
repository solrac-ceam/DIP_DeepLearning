rm result.test result.train  output.svm  model.svm result.csv
time ./DIP_DeepLearning  5 32 64 128 2 5 10 < files.list
svm-subset result.csv 1300 result.test result.train
svm-train -s 0 -t 0 -c 0.01 result.train model.svm
svm-predict result.test model.svm output.svm


