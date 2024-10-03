import random
import time
import uuid

global GLOBAL_IDS
GLOBAL_IDS = []

class Request:
    def __init__(self, requirements, priority, listener):
        self.requirements = requirements
        self.priority = priority
        self.listener = listener

    def __str__(self):
        return "Request for {} with priority {} for listener {}.".format(self.requirements, self.priority, self.listener)

class Peer:
    def __init__(self, id=None, rtt=None, hops=None, pdv=None, pl=None):
        self.rtt = rtt if rtt is not None else random.randint(0, 200)*random.random()
        self.hops = hops if hops is not None else random.randint(0, 2)
        self.pdv = pdv if pdv is not None else 0
        self.pl = pl if pl is not None else 0
        self.id = uuid.uuid4() if id is None else id
        GLOBAL_IDS.append(self.id)

    def get_id(self):
        return self.id

    def get_rtt(self, rand=True):
        if rand:
            self.rtt = self.rtt + random.randint(-1, 1)*random.random()
        if self.rtt < 0:
            self.rtt = 0
        return self.rtt

    def get_number_of_hops(self, rand=True):
        if rand:
            self.hops = random.randint(0, 2)
        return self.hops

    def get_pdv(self, rand=True):
        if rand:
            self.pdv = self.pdv + (random.randint(-1, 1)*random.random())/100.0
        if self.pdv < 0:
            self.pdv = 0

        return self.pdv

    def get_pl(self, rand=True):
        if rand:
            self.pl = self.pl + (random.randint(-1, 1)*random.random())/10000.0
        if self.pl < 0:
            self.pl = 0

        return self.pl

    def __str__(self):
        return "Peer #{} ".format(self.id)

class DegradationMethod:
    def steep(x: float, l: float):
        return (0.75/l**2) * (x-l)**2 + 0.25

    def linear(x: float, l: float):
        return -0.75*(l**(-1))*x + 1

    def smooth(x: float, l: float):
        return -(0.75/l**2) * x**2 + 1


class Node:
    CPU_CORE_VALUES = [1, 2, 4, 6, 8, 16]
    MEMORY_VALUES = [2, 4, 8, 16, 32, 64]

    def __init__(self, name, configurations=None, update=True):
        assert type(configurations) == dict or configurations is None

        self.name = name
        self.configurations = {} if configurations is None else configurations
        self.request = None
        self.update = update

        self.peers = []
        for i in range(random.randint(1, 5)):
            self.peers.append(Peer())

        # Constants definitions.
        self.CNR_THRESHOLD = 2.0/3.0
        self.MAX_PRIO = 7.0
        self.RTT_THRESHOLD = 100
        self.PDV_THRESHOLD = 2
        self.PDV_TOLERANCE = 2.0/3.0
        self.PL_THRESHOLD = 0.1
        self.PL_DEGRADATION = DegradationMethod.steep
        self.SALT = 0.0
        self.H1 = 0.25
        self.H2 = 0.25
        self.H3 = 0.25
        self.H4 = 0.25

        # Discard components.
        self.discard_c1 = False
        self.discard_c2 = False
        self.discard_c3 = False
        self.discard_c4 = False
        self.discard_c4_rtt = False
        self.discard_c4_noh = False
        self.discard_c4_pdv = False
        self.discard_c4_pl = False
        self.discard_c5 = False
        self.discard_c5_1 = False
        self.discard_c5_2 = False
        self.discard_c5_3 = False
        self.discard_c5_4 = False

        self.set_initial_state()

    def place_request(self, request):
        self.request = request
        return self.assess()

    def get_peer(self):
        return self.peers[random.randint(0, len(self.peers)-1)].get_id()

    def assess_node_resources(self, discard=False):
        if discard:
            return 1

        for requirement_topic in self.request.requirements:
            if requirement_topic not in self.configurations:
                return 0
            if self.configurations[requirement_topic] == "dds" or self.configurations[requirement_topic] == "tas" or self.configurations[requirement_topic] == "cbs":
                continue
            if self.request.requirements[requirement_topic]["value"] > self.configurations[requirement_topic]["total"]:
                return 0
        return 1

    def assess_current_node_resources_recursive(self, requirements, result, variable_result):
        # Retrieve the first requirement from the list R_0.
        first_requirement = requirements[list(requirements.keys())[0]]
        first_requirement_type = list(requirements.keys())[0]

        # Assess the first requirement into rho_0.
        rho_0 = 0.0
        if "memory" in first_requirement_type:
            rho_0 = ((self.configurations["memory"]["total"]-self.configurations["memory"]["used"])-first_requirement["value"]) / (self.configurations["memory"]["total"]-self.configurations["memory"]["used"])
        elif "cpu" in first_requirement_type:
            rho_0 = ((self.configurations["cpu"]["total"]-self.configurations["cpu"]["used-cores"])-first_requirement["value"]) / (self.configurations["cpu"]["total"]-self.configurations["cpu"]["used-cores"])
        elif "bandwidth" in first_requirement_type:
            rho_0 = ((self.configurations["bandwidth"]["total"]-self.configurations["bandwidth"]["current"])-first_requirement["value"]) / (self.configurations["bandwidth"]["total"]-self.configurations["bandwidth"]["current"])
        elif "dds" in first_requirement_type:
            rho_0 = 1 if first_requirement["value"] in self.configurations["dds"]["topics"] else 0
        elif "tas" in first_requirement_type:
            rho_0 = 1
        elif "cbs" in first_requirement_type:
            rho_0 = 1
        else:
            rho_0 = 0

        result.append(rho_0)

        # Recursive call.
        if len(requirements) == 1:
            variable_result.append(rho_0)
            return rho_0

        requirements.pop(first_requirement_type)
        this_grade = self.CNR_THRESHOLD * rho_0 + (1 - self.CNR_THRESHOLD)*self.assess_current_node_resources_recursive(requirements, result, variable_result)
        variable_result.append(this_grade)
        return this_grade

    def assess_current_node_resources(self, discard=False):
        if discard:
            return 1, []

        if self.request is None:
            return 0

        results = []
        vresults = []
        result = self.assess_current_node_resources_recursive(self.request.requirements, results, vresults)

        result = 0 if any([item == 0 for item in results]) else result

        return result, results, vresults

    def assess_fairness(self, discard=False):
        if discard:
            return 1

        if self.request is None:
            return 0

        return (self.request.priority+1) / (self.MAX_PRIO+1)

    def assess_rtt(self, x, discard=False):
        if discard:
            return 1

        if x <= self.RTT_THRESHOLD:
            return 1
        return 2.0**(-(float(x)-self.RTT_THRESHOLD)/self.RTT_THRESHOLD)

    def assess_number_of_hops(self, x, discard=False):
        if discard:
            return 1

        return 2**(-int(x))

    def assess_pdv(self, x, discard=False):
        if discard:
            return 1

        if x <= self.PDV_THRESHOLD:
            return 1
        elif x > self.PDV_THRESHOLD and x < (self.PDV_THRESHOLD + self.PDV_THRESHOLD*self.PDV_TOLERANCE):
            return -((x-self.PDV_THRESHOLD)/(self.PDV_THRESHOLD*self.PDV_TOLERANCE)) + 1
        else:
            return 0

    def assess_pl(self, x, discard=False):
        if discard:
            return 1

        if x > self.PL_THRESHOLD:
            return 0
        return self.PL_DEGRADATION(x, self.PL_THRESHOLD)

    def assess_proximity(self, discard=False):
        if discard:
            return 1, 1, 1, 1, 1, 1, 1, 1, 1

        if self.request is None:
            return 0

        peer = [peer for peer in self.peers if peer.id == self.request.listener][0]

        raw_rtt = peer.get_rtt()
        rtt = self.assess_rtt(raw_rtt, discard=self.discard_c4_rtt)

        raw_number_of_hops = peer.get_number_of_hops()
        noh = self.assess_number_of_hops(raw_number_of_hops, discard=self.discard_c4_noh)

        raw_pdv = peer.get_pdv()
        pdv = self.assess_pdv(raw_pdv, discard=self.discard_c4_pdv)

        raw_pl = peer.get_pl()
        pl = self.assess_pl(raw_pl, discard=self.discard_c4_pl)

        return (rtt+noh+pdv+pl)/4.0, rtt, noh, pdv, pl, raw_rtt, raw_number_of_hops, raw_pdv, raw_pl

    def assess_historical_1(self):
        return 1

    def assess_historical_2(self):
        return 1

    def assess_historical_3(self):
        return 1

    def assess_historical_4(self):
        return 1

    def assess_historical_performance(self, discard=False):
        if discard:
            return 1, 1, 1, 1, 1, 1

        a1 = self.assess_historical_1()
        a2 = self.assess_historical_2()
        a3 = self.assess_historical_3()
        a4 = self.assess_historical_4()

        salt = random.random()

        return (1-self.SALT)*(self.H1*a1 + self.H2*a2 + self.H3*a3 + self.H4*a4) + self.SALT*salt, self.H1*a1, self.H2*a2, self.H3*a3, self.H4*a4, salt

    def assess(self):
        # First criterion: node resources.
        node_resources = self.assess_node_resources(discard=self.discard_c1)
        if node_resources == 0:
            return 0, 0, (0, [], []), 0, (0, 0, 0, 0, 0, 0, 0, 0, 0), (0, 0, 0, 0, 0, 0)

        # Second criterion: current node resources.
        current_node_resources, cnr_results, cnr_vresults = self.assess_current_node_resources(discard=self.discard_c2)

        # Third criterion: fairness.
        fairness = self.assess_fairness(discard=self.discard_c3)

        # Fourth criterion: proximity.
        proximity, rtt, noh, pdv, pl, raw_rtt, raw_noh, raw_pdv, raw_pl = self.assess_proximity(discard=self.discard_c4)

        # Fifth criterion: historical performance.
        historical_performance, a1, a2, a3, a4, salt = self.assess_historical_performance(discard=self.discard_c5)

        return node_resources * (historical_performance+proximity)/2 * current_node_resources * fairness, node_resources, (current_node_resources, cnr_results, cnr_vresults), fairness, (proximity, rtt, noh, pdv, pl, raw_rtt, raw_noh, raw_pdv, raw_pl), (historical_performance, a1, a2, a3, a4, salt)

    def assess_raw_values(resources, current_resources, fairness, proximity, historical, constants, detail_grade=0):
        # Unpack current_resources.
        cr_value, rho_i, vrho_i = current_resources

        # Unpack proximity.
        p_value, rtt, noh, pdv, pl, raw_rtt, raw_noh, raw_pdv, raw_pl = proximity

        # Unpack historical.
        h_value, h1, h2, h3, h4, salt = historical

        # Unpack constants.
        if constants is not None:
            CNR, PRIO, RTT_T, PDV_T, PDV_TOL, PL_T, PL_D, SALT, H1, H2, H3, H4 = constants

        # Assess final grade.
        c1 = resources
        c2 = cr_value if detail_grade >= 1 else 1
        c3 = fairness if detail_grade >= 2 else 1
        c4 = p_value if detail_grade >= 3 else 1
        c5 = h_value if detail_grade >= 4 else 1

        return c1*c2*c3*(c4+c5)/2

    def update_state(self):
        if not self.update:
            return

        # Update memory.
        self.configurations["memory"]["used"] += random.randrange(-int(self.configurations["memory"]["used"]) * 50, (
                    self.configurations["memory"]["total"] - int(self.configurations["memory"]["used"])) * 50) * 0.001
        # Update CPU usage.
        self.configurations["cpu"]["used-cores"] += random.randrange(-self.configurations["cpu"]["used-cores"], (
                    self.configurations["cpu"]["total"] - self.configurations["cpu"]["used-cores"]))
        # Update bandwidth.
        self.configurations["bandwidth"]["current"] += random.randrange(-int(self.configurations["bandwidth"]["current"]) * 50, (
                self.configurations["bandwidth"]["total"] - int(self.configurations["bandwidth"]["current"])) * 50) * 0.001

    def set_initial_state(self):
        # Initialize memory values in this node.
        if "memory" not in self.configurations:
            self.configurations["memory"] = {"total": Node.MEMORY_VALUES[random.randint(0, len(Node.MEMORY_VALUES)-1)]}
        if "used" not in self.configurations["memory"]:
            self.configurations["memory"].update({"used": random.random() * self.configurations["memory"]["total"]})

        # Initialize CPU values in this node.
        if "cpu" not in self.configurations:
            self.configurations["cpu"] = {
                "total": Node.CPU_CORE_VALUES[random.randint(0, len(Node.CPU_CORE_VALUES) - 1)]}
        if "used-cores" not in self.configurations["cpu"]:
            self.configurations["cpu"].update(
                {"used-cores": int(random.random() * self.configurations["cpu"]["total"])})

        # Initialize bandwidth in this node.
        if "bandwidth" not in self.configurations:
            self.configurations["bandwidth"] = {"total": random.randrange(8, 1024)}
        if "current" not in self.configurations["bandwidth"]:
            self.configurations["bandwidth"].update({"current": random.randrange(1, int(self.configurations["bandwidth"]["total"]))})

        # Initialize DDS in this node.
        if "dds" not in self.configurations:
            self.configurations["dds"] = {"total": True if random.randint(0, 1) == 1 else False}
        if "topic" not in self.configurations["dds"]:
            self.configurations["dds"].update({"topic": ["example1", "example2", "example3"]})

        # Initialize TAS in this node.
        if "tas" not in self.configurations:
            self.configurations["tas"] = {"total": True if random.randint(0, 1) == 1 else False}
        if self.configurations["tas"]["total"]:
            return

        # Initialize CBS in this node.
        if "cbs" not in self.configurations:
            self.configurations["cbs"] = {"total": True if random.randint(0, 1) == 1 else False}
        if self.configurations["cbs"]["total"]:
            return

    def __str__(self):
        return "Node \"" + self.name + "\"\n" + \
            "  Memory: {:5.1f} of {:5.1f} GB \n".format(self.configurations["memory"]["used"],
                                                        self.configurations["memory"]["total"]) + \
            "  CPU: {:d} of {:d} cores used \n".format(self.configurations["cpu"]["used-cores"],
                                                       self.configurations["cpu"]["total"]) + \
            "  Bandwidth: {:5.1f} of {:5.1f} Mbps".format(self.configurations["bandwidth"]["current"], self.configurations["bandwidth"]["total"])


def main():
    n1 = Node("Nó 1", {"cpu": {"total": 4, "used-cores": 1}, "memory": {"total": 32, "used": 16}, "bandwidth": {"total": 1024, "current": 250}})

    while True:
        time.sleep(0.5)
        print(n1)
        request = Request({"cpu": {"value": 1}, "memory": {"value": 2}, "bandwidth": {"value": 100}}, 4, n1.peers[0].id)
        print("Placing request: {}".format(request))

        bid, a1, (a2, a2_all), a3, (a4, rtt, noh, pdv, pl, raw_rtt, raw_noh, raw_pdv, raw_pl), (a5, a51, a52, a53, a54, a5_salt) = n1.place_request(request)

        print("Assessment: ")
        print("  - a1: {}".format(a1))
        print("  - a2: {}".format(a2))
        for record in a2_all:
            print("     - item: {}".format(record))
        print("  - a3: {}".format(a3))
        print("  - a4: {}".format(a4))
        print("     - rtt: {}".format(rtt))
        print("     - raw_rtt: {}".format(raw_rtt))
        print("     - noh: {}".format(noh))
        print("     - raw_noh: {}".format(raw_noh))
        print("     - pdv: {}".format(pdv))
        print("     - raw_pdv: {}".format(raw_pdv))
        print("     - pl: {}".format(pl))
        print("     - raw_pl: {}".format(raw_pl))
        print("  - a5: {}".format(a5))
        print("     - d1: {}".format(a51))
        print("     - d2: {}".format(a52))
        print("     - d3: {}".format(a53))
        print("     - d4: {}".format(a54))
        print("     - salt: {}".format(a5_salt))
        print("Bid is: {}".format(bid))

        n1.update_state()


if __name__ == '__main__':
    main()
