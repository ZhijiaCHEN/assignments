import psycopg2, os,random, sys, csv
try:
    dbConn = psycopg2.connect(database='comment', user='zhijia', password='1', host='35.229.80.182')
    dbConn.set_session(autocommit=True)
    dbCursor = dbConn.cursor()
except (Exception, psycopg2.DatabaseError) as error:
    print(error)
    sys.exit()

width_category = {'<= 20':1, '> 20 and < 24':2, '>= 24':3}
shoulder_category = {'<= 3': 1, '> 3 and <= 6': 2, '> 6': 3}
driveways_category = {'None':1, '> 0 and <= 10':2, '> 10': 3}


f = open('Statistical Methods/PennCrash.csv')
reader = csv.reader(f)
for row in reader:
    row[1] = int(row[1]) == 1
    row[9] = width_category[row[9]]
    row[10] = row[10] == 'High'
    row[11] = shoulder_category[row[11]]
    row[12] = driveways_category[row[12]]
    row[13] = row[13] != 'None'
    row[14] = row[14] != 'None'
    dbCursor.execute('insert into penn_crash values({}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {})'.format(*row))


