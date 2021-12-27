import pandas as pd
import csv


data1 = pd.read_csv('../data/train.csv', usecols=[0, 1, 2])
data2 = pd.read_csv('../data/test.csv', usecols=[0, 1])
# print(data1.info())
# print(data1.index)
# print(data1.duplicated().sum())
# print(data1.notnull())
# print(data1.columns)
data1['content'] = data1['content'].apply(str).str.strip()
data1['comment_all'] = data1['comment_all'].apply(str).str.strip()
data1['comment_all'] = data1['comment_all'].fillna(" ")
data2['content'] = data2['content'].apply(str).str.strip()
data2['comment_all'] = data2['comment_all'].apply(str).str.strip()
data2['comment_all'] = data2['comment_all'].fillna(" ")
# print(data1.head())

# data1.to_csv("../data/train2.csv", index=0, encoding="utf_8_sig")
# data2.to_csv("../data/test2.csv", index=0, encoding="utf_8_sig")
