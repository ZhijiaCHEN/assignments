drop table if exists pub2person, publication, person cascade;
create table publication
(
	pubid int primary key,
    title varchar, 
    booktitle varchar, 
    pages varchar, 
    pubyear varchar, 
    address varchar, 
    journal varchar, 
    volume varchar, 
    pubnumber varchar, 
    pubmonth varchar, 
    url varchar, 
    ee varchar, 
    cdrom varchar, 
    cite varchar, 
    publisher varchar, 
    note varchar, 
    crossref varchar, 
    isbn varchar, 
    series varchar, 
    school varchar, 
    chapter varchar, 
    publnr varchar
);

create table person
(
	personname varchar primary key,
	affiliate varchar,
	homepage varchar
);

create table pub2person
(
	pubid int references publication,
	pubtype varchar,
	personname varchar references person,
	personrole varchar
);