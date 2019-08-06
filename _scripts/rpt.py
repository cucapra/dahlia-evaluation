import re

class RPTParser:
    """
    Class implementing parsing functionality of RPT files generated by Xilinx
    tools. The core functionality is extracting tables out of these files.
    """

    SKIP_LINE = re.compile(r'^.*(\+-*)*\+$')

    def __init__(self, filepath):
        with open(filepath, "r") as data:
            self.lines = data.read().split("\n")

    @staticmethod
    def _clean_and_strip(elems):
        "Remove all empty elements from the list and strips each string element."
        m = map(lambda e: e.strip(), elems)
        return list(filter(lambda e: e != '\n' and e != '', m))

    @staticmethod
    def _parse_simple_header(line):
        return RPTParser._clean_and_strip(line.split('|'))

    @staticmethod
    def _parse_multi_header(lines):
        """
        Extract header from the form:
        +------+--------+--------+----------+-----------+-----------+
        |      |     Latency     | Iteration|  Initiation Interval  |
        | Name |   min  |   max  |  Latency |  achieved |   target  |
        +------+--------+--------+----------+-----------+-----------+

        into: ["Name", "Latency_min", "Latency_max", "Iteration Latency", ...]

        This will fail to correctly parse this header. See the comment below
        for an explanation:
        +------+--------+--------+--------+--------+
        |      |     Latency     |     Foo         |
        | Name |   min  |   max  |   bar  |   baz  |
        +------+--------+--------+--------+--------+
        turns into: ["Name", "Latency_min", "Latency_max", "Latecy_bar", "Latency_baz", "Foo"]
        """

        multi_headers = []
        secondary_hdrs = lines[1].split('|')

        # Use the following heuristic to generate header names:
        # - If header starts with a small letter, it is a secondary header.
        # - If the last save sequence of headers doesn't already contain this
        #   header name, add it to the last one.
        # - Otherwise add a new sub header class.
        for idx, line in enumerate(secondary_hdrs, 1):
            clean_line = line.strip()
            if len(clean_line) == 0: continue
            elif (clean_line[0].islower()
                  and len(multi_headers) > 0
                  and multi_headers[-1][0].islower()
                  and clean_line not in multi_headers[-1]):
                multi_headers[-1].append(clean_line)
            else: multi_headers.append([clean_line])

        # Extract base headers and drop the starting empty lines and ending '\n'.
        base_hdrs = lines[0].split('|')[1:-1]

        if len(base_hdrs) != len(multi_headers):
            raise Exception(
                "Something went wrong while parsing multi header " +
                "base len: {}, mult len: {}".format(
                    len(base_hdrs),
                    len(multi_headers)))

        hdrs = []
        for idx in range(0, len(base_hdrs)):
            for mult in multi_headers[idx]:
                hdrs.append((base_hdrs[idx].strip() + " " + mult).strip())

        return hdrs

    @staticmethod
    def _parse_table(table_lines, multi_header):
        """
        Parses a simple table of the form:
        +--------+-------+----------+------------+
        |  Clock | Target| Estimated| Uncertainty|
        +--------+-------+----------+------------+
        |ap_clk  |   7.00|      4.39|        1.89|
        |ap_clk  |   7.00|      4.39|        1.89|
        +--------+-------+----------+------------+
        |ap_clk  |   7.00|      4.39|        1.89|
        +--------+-------+----------+------------+

        The might be any number of rows after the headers. The input parameter
        is a list of lines of the table starting with the top most header line.
        Returns a list of list with all the fields and the headers.

        """

        table = []

        # Extract the headers and set table start
        table_start = 0
        if multi_header:
            table.append(RPTParser._parse_multi_header(table_lines[1:3]))
            table_start = 3
        else:
            table.append(RPTParser._parse_simple_header(table_lines[1]))
            table_start = 2

        for line in table_lines[table_start:]:
            if not RPTParser.SKIP_LINE.match(line):
                table.append(RPTParser._clean_and_strip(line.split('|')))

        return table

    def get_table(self, reg, off, multi_header=False):
        start = 0
        end = 0
        for idx, line in enumerate(self.lines, 1):
            if reg.search(line):
                start = idx + off
                end = start
                while(self.lines[end].strip() != ''): end += 1

        return self._parse_table(self.lines[start:end], multi_header)
