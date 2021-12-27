import pandas as pd
import numpy as np
from sklearn.feature_extraction.text import CountVectorizer
from sklearn.model_selection import train_test_split
from sklearn.naive_bayes import MultinomialNB
from sklearn.naive_bayes import BernoulliNB
from sklearn.feature_extraction.text import TfidfTransformer
from sklearn.metrics import f1_score


def validate(train_sample, train_label):
    train_sample, test_sample, train_label, test_label = train_test_split(train_sample, train_label, test_size=0.3)
    vec = CountVectorizer()
    X_train = vec.fit_transform(train_sample)
    X_test = vec.transform(test_sample)

    tfidf = TfidfTransformer()

    X_train = tfidf.fit_transform(X_train)
    X_test = tfidf.transform(X_test)

    mnb = MultinomialNB(alpha=0.03)
    # mnb = BernoulliNB(alpha=0.03)
    mnb.fit(X_train, train_label)
    score = mnb.score(X_test, test_label)
    print("Accuracy: ", score)

    y_pred = mnb.predict(X_test)
    f1_micro = f1_score(y_pred, test_label, average='micro')
    f1_macro = f1_score(y_pred, test_label, average='macro')

    print('f1_micro: {0}'.format(f1_micro))
    print('f1_macro: {0}'.format(f1_macro))


def predict(train_sample, train_label, test_sample):
    vec = CountVectorizer()
    X_train = vec.fit_transform(train_sample)
    X_test = vec.transform(test_sample)

    tfidf = TfidfTransformer()

    X_train = tfidf.fit_transform(X_train)
    X_test = tfidf.transform(X_test)

    mnb = MultinomialNB(alpha=0.02)
    # mnb = BernoulliNB(alpha=0.03)
    mnb.fit(X_train, train_label)
    pred = mnb.predict(X_test)
    with open('bayes_ans.txt', 'w') as f:
        for x in np.nditer(pred):
            f.write(str(x))
            f.write('\n')


def main():
    train_sample = np.array(
        pd.read_csv('../data/train1.csv', dtype=str, encoding='utf-8', usecols=[0]).values.tolist())
    train_label = np.array(
        pd.read_csv('../data/train1.csv', dtype=str, encoding='utf-8', usecols=[1]).values.tolist())
    test_sample = np.array(
        pd.read_csv('../data/test1.csv', dtype=str, encoding='utf-8', usecols=[0]).values.tolist())
    # print(train_sample[10])
    # print(train_label[10])
    # print(test_sample[10])
    validate(train_sample.ravel(), train_label.ravel())
    # predict(train_sample.ravel(), train_label.ravel(), test_sample.ravel())


if __name__ == "__main__":
    main()
