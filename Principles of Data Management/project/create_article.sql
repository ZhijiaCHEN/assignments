drop table if exists article;
create table article
(
	author varchar references author, 
    editor varchar, 
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
    publnr varchar,
	primary key(author, isbn)
);