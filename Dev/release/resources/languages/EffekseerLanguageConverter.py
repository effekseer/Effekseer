import csv
import json
import collections as cl
import codecs

values_en = cl.OrderedDict()
values_ja = cl.OrderedDict()
kv_en = {}
kv_ja = {}

csv_files = ['Effekseer.csv']

line = 0
for csv_file in csv_files:
    with open(csv_file, 'r', encoding="utf8") as f:
        reader = csv.reader(f)
        header = next(reader)
        for row in reader:
            if len(row) < 3:
                print('error ' + str(line))
                break
            kv_en[str(row[0])] = str(row[1]).replace('\r\n',r'\n').replace('\n',r'\n')
            kv_ja[str(row[0])] = str(row[2]).replace('\r\n',r'\n').replace('\n',r'\n')
            line += 1

values_en['kv'] = kv_en
values_ja['kv'] = kv_ja

#f_en = codecs.open('effekseer_en.json','w', "utf-8")
#json.dump(values_en,f_en, ensure_ascii=False, indent=2)

#f_ja = codecs.open('effekseer_ja.json','w', "utf-8")
#json.dump(values_ja,f_ja, ensure_ascii=False, indent=2)

ftxt_en = codecs.open('effekseer_en.txt','w', "utf-8")
for k,v in kv_en.items():
    ftxt_en.write(str(k) + ',' + str(v) + '\r\n')

ftxt_ja = codecs.open('effekseer_ja.txt','w', "utf-8")
for k,v in kv_ja.items():
    ftxt_ja.write(str(k) + ',' + str(v) + '\r\n')
