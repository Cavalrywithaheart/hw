import pandas as pd
import csv


data1 = pd.read_csv('../data/train.csv', usecols=[0, 1, 2])
data2 = pd.read_csv('../data/test.csv', usecols=[0, 1])
# print(data.info())
# print(data.index)
# print(data.duplicated().sum())
# print(data.notnull())
# print(data.columns)
data1['content'] = data1['content'].fillna(" ")
data1['comment_all'] = data1['comment_all'].fillna(" ")
data1['content'] = data1[['content', 'comment_all']].astype(str).apply(' '.join, axis=1)
data1 = data1.drop(labels=['comment_all'], axis=1)
data1['content'] = data1['content'].apply(str).str.strip()

data2['content'] = data2['content'].fillna(" ")
data2['comment_all'] = data2['comment_all'].fillna(" ")
data2['content'] = data2[['content', 'comment_all']].astype(str).apply(' '.join, axis=1)
data2 = data2.drop(labels=['comment_all'], axis=1)
data2['content'] = data2['content'].apply(str).str.strip()

# print(data2.head)
data1.to_csv("../data/train1.csv", index=0, encoding="utf_8_sig")
data2.to_csv("../data/test1.csv", index=0, encoding="utf_8_sig")
