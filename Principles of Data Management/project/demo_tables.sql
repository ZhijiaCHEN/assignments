drop table if exists publication_demo;
create table publication_demo as select * from publication where false;

drop table if exists person_demo;
create table person_demo as select * from person where false;

drop table if exists pub2person_demo;
create table pub2person_demo as select * from pub2person where false;

drop table if exists abstract_demo;
create table abstract_demo as select * from abstract where false;
insert into abstract_demo(pubid, title, pubyear) select pubid, title, pubyear::int from publication;

alter table abstract_demo drop column abstract;
alter table abstract_demo add column abstract varchar;