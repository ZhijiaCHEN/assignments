alter table publication drop column if exists abstract;
alter table publication add column abstract varchar;

create or replace function get_my_results() returns void as
$$
declare
	itr record;
begin
	for itr in (select pubid, abstract from abstract) loop
		update publication set abstract = itr.abstract where pubid = itr.pubid;
	end loop;
end;
$$
language plpgsql;
select get_my_results();

drop table if exists jeff_results;

create table jeff_results
(
	pubkey varchar,
	title varchar,
	abstract varchar,
	name_id varchar,
	primary key (title, pubkey)
);

/*delete duplicate titles to use title as primary key*/
delete from jeff_results  tmp where (select count(*) from jeff_results where title = tmp.title) > 1;

create index title_idx on publication(title);/*create index on title to accelarate updating abstract by title, takes around 50s.*/
cluster publication using title_idx;/*takes around 60s*/

create or replace function get_jeff_results() returns void as
$$
declare
	itr record;
begin
	for itr in (select * from jeff_results) loop
		update publication set abstract = itr.abstract where title = itr.title and pubyear = '2009';
	end loop;
end;
$$
language plpgsql;
select get_jeff_results();/*takes around 5s*/

/*restored the clustering order*/
create index pubid_idx on publication(pubid);
cluster publication using pubid_idx;/*90s*/

drop index pubid_idx, title_idx;

update publication set abstract = lpad('', floor(random()*(2000-500+1))::integer + 500, 'a') where pubid in (select pubid from  publication where abstract is null limit 100000);