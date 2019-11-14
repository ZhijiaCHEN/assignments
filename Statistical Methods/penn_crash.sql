DROP TABLE IF EXISTS penn_crash;
CREATE TABLE penn_crash(
    id INTEGER PRIMARY KEY,
    treat BOOLEAN,
    tot04 INTEGER,
    tot08 INTEGER,
    tot13 INTEGER,
    adt04 FLOAT,
    adt08 FLOAT,
    adt12 FLOAT,
    segment_length FLOAT,
    width_category INTEGER,
    speed_grt_45 BOOLEAN,
    shoulder_category INTEGER,
    driveways_category INTEGER,
    intersection_inclusion BOOLEAN,
    curve_existence BOOLEAN,
    curve_degree FLOAT
);