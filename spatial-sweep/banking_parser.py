import os
from html.parser import HTMLParser

# create a subclass and override the handler methods
class BankingDecisionParser(HTMLParser):
    def __init__(self,g, dirname='data'):
        HTMLParser.__init__(self)
        self.par = str(g)
        self.write = 0
        self.ram = None
        self.table = {"a_sram":0,"b_sram":0}
        self.bestc = 0
        self.bank = {'N':[0,0],'B':[0,0], 'alpha':[0,0], 'P':[0,0], 'dup':0}
        self.i = 0
        self.store = {"a_sram":"input_matrix_a", "b_sram":"input_matrix_b"}
        self.dirname = dirname

        # Write to the csv.
        for name in ["a_sram", "b_sram"]:
            csv_name = os.path.join(self.dirname, self.store[name] + ".csv")
            if not os.path.exists(csv_name):
                with open(csv_name, "w") as f:
                    for term in ['par','N','B', 'alpha', 'P', 'dup']:
                        f.write(term)
                        if term != 'dup': f.write(', ')
                    f.write('\n')

    def handle_starttag(self, tag, attrs):
        if self.ram is not None and tag == 'table':
            self.table[self.ram] += 1

    def handle_endtag(self, tag):
        if self.ram is not None and tag == 'table':
            self.table[self.ram] -= 1
            if self.table[self.ram] == 0:
                self.ram = None

    def handle_data(self, data):
        if "a_sram" in data and 'Cost' in data:
            self.ram = "a_sram"
            self.bestc = float(data.split('Cost ')[1].split(' (')[0])
        if "b_sram" in data and 'Cost' in data:
            self.ram = "b_sram"
            self.bestc = float(data.split('Cost ')[1].split(' (')[0])
        if  'scheme cost' in data and self.bestc==True:
            self.bestc = 0
        if self.ram is not None and 'scheme cost' in data:
            if round(float(data.split('scheme cost ')[1].split('%:')[0]),2) == self.bestc:
                self.bestc = True
        if self.bestc==True and 'Banking Decision' in data:
            dims = data.split('Dims {')[1].split('}:')[0].split(',')
            self.i+=1
        if self.i == 1:
            for j in range(len(dims)):
                self.bank['N'][j] = int(data.split('Cyclic: N=')[1].split(', B')[0])
                self.bank['B'][j] = int(data.split(', B=')[1].split(', alpha')[0])
        if self.i == 3:
            alpha = data.split('>,')[0].split(',')
            for j,a in enumerate(alpha):
                self.bank['alpha'][j] = int(a)
        if self.i == 5:
            Ps = data.split('>')[0].split(',')
            for j,p in enumerate(Ps):
                self.bank['P'][j] = int(p)
            if len(Ps) == 2:
                if self.write < 2:
                    with open(os.path.join(self.dirname, self.store[self.ram]+".csv"),
                              "a+") as f:
                        f.write(self.par)
                        f.write(', ')
                        for term in ['N','B', 'alpha', 'P', 'dup']:
                            f.write(str(self.bank[term]).replace(',',''))
                            if term != 'dup': f.write(', ')
                        f.write('\n')
                self.write+=1
                self.i = 0
            else:
                self.i = 1
        elif self.i != 0:
            self.i+=1
        if self.bestc and 'duplicates' in data:
            self.bank['dup'] = int(data.split(' dup')[0])
