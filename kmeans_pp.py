import pandas as pd
import numpy as np
import sys
import copy
import mykmeanssp as km

def main():
    np.random.seed(1234)
    if (len (sys.argv) > 6 or len(sys.argv) < 5):
        print("an error occurred")
        sys.exit(1)
    if (len(sys.argv) == 6):
        if not is_valid_int(sys.argv[1]):
            print("Invalid number of clusters!")
            sys.exit(1)
        k = int(float(sys.argv[1]))
        if not is_valid_int(sys.argv[2]):
            print("Invalid maximum iteration!")
            sys.exit(1) 
        iter = int(float(sys.argv[2]))
        if ((iter <= 1) or (iter>=1000)):
            print("Invalid maximum iteration!")
            sys.exit(1)
        try:
            epsilon = float(sys.argv[3])
            if(epsilon < 0):
                print("Invalid epsilon!")
                sys.exit(1)
        except ValueError:
            print("Invalid epsilon!")
            sys.exit(1)
        file_path_1 = sys.argv[4]
        file_path_2 = sys.argv[5]
    else:
        if not is_valid_int(sys.argv[1]):
            print("an error occurred")
            sys.exit(1)
        k = int(float(sys.argv[1]))
        try:
            epsilon = float(sys.argv[2])
            if(epsilon < 0):
                print("Invalid epsilon!")
                sys.exit(1)
        except ValueError:
            print("Invalid epsilon!")
            sys.exit(1)
        iter = 300
        file_path_1 = sys.argv[3]
        file_path_2 = sys.argv[4]

    df= input_to_dataframe(file_path_1, file_path_2)
    nc_points = copy.deepcopy(df)
    index = np.random.choice(nc_points.index)
    if (k >= nc_points["key"].size or k <= 1):
        print("Invalid number of clusters!")
        sys.exit(1)
    centroids = [df.loc[index].values[1:]]
    key_array = [int(nc_points.loc[index].key)]
    nc_points.drop(index, inplace = True)
    nc_points["distance"] = None
    for i in range(k-1):
        distance_calculation(centroids, nc_points)
        sum = nc_points["distance"].sum()
        nc_points["probability"] = nc_points["distance"]/sum
        index = np.random.choice(nc_points.index, p = nc_points["probability"])
        nc_points.drop("probability", axis = 1, inplace = True)
        centroids.append(nc_points.loc[index].values[1:nc_points.loc[index].values.size-1])
        key_array.append(int(nc_points.loc[index].key))
        nc_points.drop(index, inplace = True)

    for i in range(len(centroids)):
        centroids[i] = centroids[i].tolist()
    for centroid in centroids:
        for i in range(len(centroid)):
            centroid[i] = float(centroid[i])

    df.drop("key", axis = 1, inplace = True)
    points = df.values.tolist()

    for point in points:
        for i in range(len(point)):
            point[i] = float(point[i])
    n = len(points)
    d = len(points[0])
    result = km.fit(n,k,iter,d,epsilon,centroids,points)
    if(len(key_array) == 1):
        print(key_array[0])
    else:
        for i in range(len(key_array)-1):
            print(key_array[i], end = ',')
        print(key_array[i+1])

    for i in range(len(result)):
        for j in range(len(result[i])-1):
            print("%.4f" % result[i][j], end = ',')
        print("%.4f" % result[i][j+1])




def distance_calculation(centroids, df):
    dmin = 0
    for index, row in df.iterrows():
        row_as_array = row.values[1:len(row.values)-1]
        for i in range(len(centroids)):
            d = np.sqrt(np.sum((row_as_array - centroids[i])**2))
            if (i == 0):
                dmin = d
            elif (d < dmin):
                dmin = d
        df.at[index, "distance"] = dmin
    return None


def input_to_dataframe(file_path_1, file_path_2):
    df1 = pd.read_csv(file_path_1, header=None)
    df2 = pd.read_csv(file_path_2, header=None)
    df1.rename(columns = {0: "key"}, inplace = True)
    df2.rename(columns = {0: "key"}, inplace = True)
    merged_df=pd.merge(df1, df2, on='key')
    merged_df.sort_values(by='key', inplace=True)
    merged_df.reset_index(drop=True, inplace=True)
    return merged_df

       





def is_valid_int(value):
    for i in range(len(value)):
        if (not value[i].isdigit()) and (value[i] != "."):
            return False
        elif (value[i] == "."):
            if(i == (len(value) -1)):
                return False
            i+=1
            while (i < len(value)):
                if (not (value[i] == '0')):
                    return False
                i+=1

            break
    return True

if (__name__ == "__main__"):
    main()