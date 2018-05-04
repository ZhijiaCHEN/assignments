import scrapy
from scrapy_splash import SplashRequest
from scrapy_splash import SlotPolicy
from scrapy.selector import Selector
from scrapy.http import HtmlResponse
import psycopg2
import re
import time
import random
class QuotesSpider(scrapy.Spider):
    requstCnt = 0
    hitCnt = 0
    name = "quotes"
    fileName = 1
    pubyear = 2005
    conn = psycopg2.connect("dbname=postgres user=postgres password=postgres")
    conn.autocommit = True
    cur = conn.cursor()
    #cur.execute("select pubid, title from publication where pubyear = '{0}';".format(pubyear))
    #records = cur.fetchall()
    #totalRequst = len(records)
    searchUrlHead = 'https://dl.acm.org/results.cfm?query=acmdlTitle:('
    searchUrlTail = ')&within=owners.owner=HOSTED&filtered=&dte={0}&bfr={0}'.format(pubyear)
    def start_requests(self):
        for i in range(450, 1001):
            time.sleep(float(random.randint(100,2000))/1000)
            yield scrapy.Request(self.gen_page_url(i*20), callback=self.parsePage, meta = {'pageNum': i})
            #yield SplashRequest(self.gen_page_url(i*20), self.parsePage, endpoint='render.html')

            self.cur.execute("select count(pubid) from abstract_demo where pubyear = 2005 and abstract is not null")
            cnt = self.cur.fetchone()
            if cnt[0] > 10000:
                return
            
        #for record in self.records:
        #    yield scrapy.Request(self.gen_search_url(record[1]), callback=self.parse, meta={'pubid':record[0]})
        # yield scrapy.Request(url=url, callback=self.parse)
        #hile record != None:
        #    self.pubid = record[0]
        #    yield SplashRequest(self.gen_search_url(record[1]), self.parse, endpoint='render.html', args={'wait': 30})
        #    record = self.cur.fetchone()

    def parse(self, response):
        #page = response.url.split("/")[-2]
        #filename = 'quotes-%s.html' % page
        #self.log(response.xpath('//div[@style="display:inline"]').extract_first())
        #self.log('get response for pubid {0} from url: {1}'.format(response.meta['pubid'], response.url))
        results = response.xpath('//a[@target="_self"]')
        if len(results) > 0:
            self.log(results[0].xpath('@href').extract_first())
            cid = [int(i) for i in re.findall(r'-?\d+\.?\d*', results[0].xpath('@href').extract_first())]
            if len(cid) > 0:
                #yield SplashRequest(self.gen_paper_url(cid[0]), self.extact_abstract, endpoint='render.html', args={'wait': 5})
                yield scrapy.Request(self.gen_paper_url(cid[0]), self.extact_abstract, meta = {
                    'splash':{
                        'args':{
                            'wait': 5
                        },
                        'endpoint': 'render.html',
                        'slot_policy': SlotPolicy.PER_DOMAIN,
                    },
                    'pubid': response.meta['pubid']}
                    )
            else:
                with open(str(self.fileName)+'.html', 'w+') as f:
                    f.write('pubid {0} has a search result does not have a citation id!'.format(response.meta['pubid']))
            #f.dump(response, f)
        #self.log('-----------------------------------------------Saved file %s' % str(self.fileName))
        self.requstCnt += 1
        self.log('requst cnt: {0}, current pubid: {1}'.format(self.requstCnt, response.meta['pubid']))
        time.sleep(0.2)

    def parsePage(self, response):
        self.log('current page: {0}'.format(response.meta['pageNum']))
        results = response.xpath('//a[@target="_self"]')
        if len(results) > 0:
            for elm in results:
            #for i in range(len(results)):
            #    if i < 5:
            #        elm = results[i]
                cid = [int(i) for i in re.findall(r'-?\d+\.?\d*', elm.xpath('@href').extract_first())]
                time.sleep(float(random.randint(100,2000))/1000)
                if len(cid) > 0:
                    #self.log('request cid: {0} with url: {1}'.format(cid[0], self.gen_paper_url(cid[0])))
                    yield SplashRequest(self.gen_paper_url(cid[0]), self.extact_abstract, endpoint='render.html', args={'wait': 10})
                else:
                    self.log('can not get cid from: {0}'.format(elm.xpath('@href').extract_first()))

    def extact_abstract(self, response):
        abstract = response.xpath('//div[@style="display:inline"]').extract_first()
        title = response.xpath('//title/text()').extract_first()
        if abstract != None:
            #self.log('got abstract: {0} for title: {1}'.format(abstract, response.xpath('//title/text()').extract_first()))
            titleLike = '%{0}%'.format(title)
            self.cur.execute("select pubid from abstract_demo where pubyear = (%s) and title like (%s);",(self.pubyear, titleLike))
            matchedPid = self.cur.fetchall()
            matchCnt = len(matchedPid)
            if matchCnt > 1:
                self.log('title: {0} hit multile records.'.format(title))
            elif matchCnt == 1:
                self.log('hit title: {0}'.format(title))
                self.cur.execute("update abstract_demo set abstract = (%s) where pubid = (%s);",(abstract, matchedPid[0][0]))
            else:
                self.log('missed title: {0}'.format(response.xpath('//title/text()').extract_first()))
            #self.log('hit pubid: {0}, abstract: {1}'.format(response.meta['pubid'], abstract))
            #self.log(response.xpath('//div[@style="display:inline"]').extract_first())
            #with open(str(response.xpath('//title/text()').extract_first())+'.html', 'wb') as f:
            #        f.write(response.body)
            #        self.fileName = self.fileName+1
        else:
            self.log('none abstract title: {0}'.title)
            with open(title+'.html', 'wb') as f:
                f.write(response.body)

    def gen_search_url(self, title):
        #return self.searchUrlHead + "%252B" + title.replace(" ", "%20%252B") + self.searchUrlTail
        return 'https://dl.acm.org/results.cfm?query=within%3Downers%2Eowner%3DHOSTED&start={0}&filtered=&within=owners%2Eowner%3DHOSTED&dte=2007&bfr=2007&srt=%5Fscore'.format(title)
    
    def gen_page_url(self, start):
        #return self.searchUrlHead + "%252B" + title.replace(" ", "%20%252B") + self.searchUrlTail
        return 'https://dl.acm.org/results.cfm?query=%28a%29&start={0}&filtered=&within=owners%2Eowner%3DHOSTED&dte={1}&bfr={1}&srt=%5Fscore'.format(start, self.pubyear)

    def gen_paper_url(self, cid):
        return 'https://dl.acm.org/citation.cfm?id={0}'.format(cid)
        

