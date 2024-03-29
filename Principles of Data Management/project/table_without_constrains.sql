drop table if exists pub2person, publication, person cascade;
create table publication
(
	pubID int,
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
	person_name varchar,
	affiliate varchar,
	home_page varchar
);

create table pub2person
(
	pub_id int,
	pub_type varchar,
	person_name varchar,
	person_role varchar
);