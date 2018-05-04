create extension if not exists intarray;
drop table if exists queries;
create table queries
(
	qid serial,
	query text,
	cnt int default 5
);
insert into queries(query) values
('select * from publication;'),

('select * from publication where title is not null and (pubyear::integer = 2000 or pubyear::integer = 1999) and isbn is null and school is not null;'),

('select count(title) from ((publication join pub2person on publication.pubid = pub2person.pubid)join person on pub2person.personname = person.personname) where personrole = ''editor'''),

('select pubyear, title, cite from publication order by pubyear;'),

('select pubyear, count(pubnumber::integer), sum(pubnumber::integer), avg(pubnumber::integer), max(pubnumber::integer) from publication where pubnumber ~ E''^\\d+$'' group by pubyear;'),

('select pubyear, count(pubnumber::integer), sum(pubnumber::integer), avg(pubnumber::integer), max(pubnumber::integer) from publication where pubnumber ~ E''^\\d+$'' group by pubyear having pubyear::integer > 2000;;');

drop table if exists performance;
create table performance
(
    num serial,
    qid int not null,
    shared_hit int not null,
    shared_read int not null,
    local_hit int not null,
    local_read int not null,
    exe_time real not null
);

set max_parallel_workers_per_gather = 4;

create or replace function exe_query(pick int) returns void as
$$
declare
	arow RECORD;
	info varchar;
    sHit varchar;
    sRead varchar;
    lHit varchar;
    lRead varchar;
    execution_time varchar;
begin
	select * into arow from queries where qid = pick;
	execute(format('explain (analyze, buffers true, format json) %s', arow.query)) into info;
    sHit := json_extract_path(info::json->0, 'Plan', 'Shared Hit Blocks');
    sRead := json_extract_path(info::json->0, 'Plan', 'Shared Read Blocks');
    lHit := json_extract_path(info::json->0, 'Plan', 'Local Hit Blocks');
    lRead := json_extract_path(info::json->0, 'Plan', 'Local Hit Blocks');
    execution_time := json_extract_path(info::json->0, 'Execution Time');
    insert into performance(qid, shared_hit, shared_read, local_hit, local_read, exe_time) values(pick, sHit::int, sRead::int, lHit::int, lRead::int, execution_time::real);
	raise notice 'Shared Hit Blocks: %, Shared Read Blocks = %, Local Hit Blocks = %, Local Hit Blocks = %, Execution Time = %', sHit::int, sRead::int, lHit::int, lRead::int, execution_time::real;
    /*raise notice 'return %', info::json->0;*/
end;
$$
language plpgsql;

create or replace function random_exe(runds int) returns void as
$$
declare
	pick INT;
	N INT;
	idArray INT[];
	arow RECORD;
	info RECORD;
begin
	select array_agg(qid) into idArray from queries where cnt >= runds;
	if icount(idArray) > 0 then
	    N := icount(idArray);
	    pick := idArray[(select (round(random()*N)::integer%N)+1)];	
        raise notice 'query % is selected.', pick;
	    PERFORM exe_query(pick);
	    update queries set cnt = cnt - 1 where qid = pick;
    else
        raise notice 'all queries have been executed.';
		return;
    end if;
	return;
end;
$$
language plpgsql;
	
