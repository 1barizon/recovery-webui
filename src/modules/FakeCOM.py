"""
Módulo de comunicação serial simulada para testes sem hardware.

Fornece a classe FakeCom que emula a comunicação serial do receptor LoRa
com dados de telemetria sintéticos no formato v2.0 (24 campos).

Suporta duas fontes de dados:
  * dados padrão embutidos (subida -> apogeu -> descida -> solo);
  * voos simulados carregados de CSV de telemetria via
    load_simulated_flight(), que converte o CSV para o mesmo formato v2.0.
"""

try:
    from .SerialCOM import BaseCom
except ImportError:
    # pyserial nao instalado — FakeCom funciona sem serial real
    class BaseCom:  # type: ignore
        """Stub para quando pyserial nao esta disponivel."""
        def __init__(self, *args, **kwargs):
            raise ImportError(
                "pyserial nao instalado. Use --simulation ou pip install pyserial"
            )

import csv
import logging
import os
from typing import List, Optional

# Pasta padrão com voos simulados (CSV).
DEFAULT_FLIGHTS_DIR = os.path.join(
    os.path.dirname(os.path.abspath(__file__)), "simulated_flights"
)

# Campos do CSV de telemetria simulada esperados como entrada.
FLIGHT_CSV_FIELDS = (
    "time,altp,temp,p,gx,gy,gz,ax,ay,az,vz,alt,lat,lon"
)


class FakeCom(BaseCom):
    """
    Emulador de comunicação serial para testes sem antena/receptor.

    Gera dados sintéticos no formato v2.0 com TEAM_ID #11 (foguete),
    simulando um pacote a cada chamada de read_response() em ciclo.
    """

    def __init__(
        self,
        logger_: logging.Logger = logging.getLogger(__name__),
        lines: Optional[List[str]] = None,
    ):
        self._is_open = False
        self._fake_port = "fake-1"

        if lines:
            self._fake_lines = lines
        else:
            self._fake_lines = self._default_lines()

        self._index = 0

        # Não chama super().__init__ — não precisamos de serial real
        self.logger = logger_
        self.logger.debug("antena sintética criada (%d pacotes)", len(self._fake_lines))

    @staticmethod
    def _default_lines() -> List[str]:
        # Dados sintéticos no formato v2.0 (24 campos)
        # TEAM_ID,millis,count,altp,temp,umi,p,gx,gy,gz,ax,ay,az,
        #   vz,maxAltitude,state,hora,data,alt,lat,lon,sat,parachute,rssi
        return [
            # Subindo
            "#11,1000,1,0.00,25.30,60.50,960.00,0.01,-0.02,0.01,0.10,0.20,-9.80,0.00,0.00,0,143000,22072026,478.00,-21.94305,-48.95409,10,0,-45",
            "#11,1100,2,5.20,25.32,60.48,959.88,0.02,-0.03,0.02,0.11,0.21,-9.79,0.05,5.20,0,143001,22072026,483.20,-21.94304,-48.95408,10,0,-45",
            "#11,1200,3,10.50,25.28,60.52,959.72,0.01,-0.01,0.01,0.09,0.19,-9.78,0.10,10.50,0,143002,22072026,488.50,-21.94303,-48.95407,10,0,-46",
            "#11,1300,4,15.80,25.25,60.55,959.55,0.03,-0.04,0.02,0.12,0.22,-9.77,0.15,15.80,0,143003,22072026,493.80,-21.94302,-48.95406,10,0,-46",
            "#11,1400,5,21.10,25.20,60.58,959.38,0.02,-0.02,0.01,0.10,0.20,-9.76,0.20,21.10,0,143004,22072026,499.10,-21.94301,-48.95405,10,0,-46",
            "#11,1500,6,26.40,25.18,60.60,959.20,0.01,-0.03,0.02,0.11,0.21,-9.75,0.25,26.40,0,143005,22072026,504.40,-21.94300,-48.95404,10,0,-47",
            "#11,1600,7,31.70,25.15,60.62,959.02,0.02,-0.01,0.01,0.09,0.19,-9.74,0.30,31.70,0,143006,22072026,509.70,-21.94299,-48.95403,10,0,-47",
            "#11,1700,8,37.00,25.12,60.65,958.85,0.01,-0.02,0.02,0.10,0.20,-9.73,0.35,37.00,0,143007,22072026,515.00,-21.94298,-48.95402,10,0,-47",
            "#11,1800,9,42.30,25.08,60.68,958.68,0.03,-0.03,0.01,0.11,0.21,-9.72,0.40,42.30,0,143008,22072026,520.30,-21.94297,-48.95401,10,0,-48",
            "#11,1900,10,47.60,25.05,60.70,958.50,0.02,-0.02,0.02,0.10,0.20,-9.71,0.45,47.60,0,143009,22072026,525.60,-21.94296,-48.95400,10,0,-48",
            # Apogeu (altp ~500m + 478m base = 978m)
            "#11,5000,35,500.00,22.80,62.10,940.00,0.01,-0.01,0.01,0.08,0.18,-9.70,0.10,500.00,3,143030,22072026,978.00,-21.94280,-48.95385,9,0,-50",
            # Descendo com paraquedas
            "#11,8000,60,400.00,23.50,61.50,945.00,0.02,-0.02,0.01,0.09,0.19,-8.50,-5.00,500.00,5,143100,22072026,878.00,-21.94260,-48.95370,8,1,-52",
            "#11,11000,85,300.00,24.10,61.00,950.00,0.01,-0.01,0.02,0.08,0.18,-7.80,-4.50,500.00,5,143130,22072026,778.00,-21.94240,-48.95355,8,1,-55",
            "#11,14000,110,200.00,24.60,60.50,955.00,0.01,-0.02,0.01,0.07,0.17,-8.10,-3.80,500.00,6,143200,22072026,678.00,-21.94220,-48.95340,9,1,-58",
            "#11,17000,135,100.00,25.00,60.00,960.00,0.02,-0.01,0.01,0.08,0.18,-8.30,-2.50,500.00,6,143230,22072026,578.00,-21.94200,-48.95325,9,1,-60",
            "#11,20000,160,50.00,25.20,59.80,962.00,0.01,-0.01,0.01,0.07,0.17,-8.50,-1.00,500.00,7,143300,22072026,528.00,-21.94180,-48.95310,10,1,-62",
            # Solo
            "#11,23000,185,0.00,25.30,59.50,965.00,0.01,-0.02,0.01,0.06,0.16,-9.80,0.00,500.00,0,143330,22072026,478.00,-21.94160,-48.95295,10,0,-65",
        ]

    # ── Controle de conexão ──────────────────────────────────────────────

    def open(self) -> bool:
        self._is_open = True
        self.logger.info("conexão serial simulada aberta")
        return True

    def close(self):
        self._is_open = False
        self.logger.info("conexão serial simulada fechada")

    def check_connected(self) -> bool:
        return self._is_open

    # ── Leitura ──────────────────────────────────────────────────────────

    def read_response(self) -> Optional[str]:
        if not self._is_open:
            self.logger.error("conexão serial simulada fechada")
            return None

        line = self._fake_lines[self._index]
        self._index = (self._index + 1) % len(self._fake_lines)
        return line

    # ── Getters / Setters ────────────────────────────────────────────────

    def get_port(self) -> Optional[str]:
        return self._fake_port

    def get_baudrate(self) -> int:
        return 115200

    def get_timeout(self) -> float:
        return 1.0

    def set_port(self, port: str) -> None:
        self.logger.info(f"definindo porta simulada como {port}")
        self._fake_port = port

    def set_baudrate(self, baudrate: int) -> None:
        self.logger.info(f"definindo baudrate simulado como {baudrate}")

    def set_timeout(self, timeout: float) -> None:
        self.logger.info(f"definindo timeout simulado como {timeout}")

    # ── Opções ───────────────────────────────────────────────────────────

    def get_port_options(self) -> list:
        return ["fake-1", "fake-2", "fake-3"]


# ══════════════════════════════════════════════════════════════════════════
# Carregamento de voos simulados (CSV)
# ══════════════════════════════════════════════════════════════════════════

def list_simulated_flights(directory: Optional[str] = None) -> List[str]:
    """Lista os arquivos CSV disponíveis na pasta de voos simulados."""
    directory = directory or DEFAULT_FLIGHTS_DIR
    if not os.path.isdir(directory):
        return []
    return sorted(
        f
        for f in os.listdir(directory)
        if f.lower().endswith(".csv") and os.path.isfile(os.path.join(directory, f))
    )


def resolve_flight_path(name: Optional[str], directory: Optional[str] = None) -> str:
    """
    Resolve o caminho de um voo simulado.

    - ``name`` pode ser um caminho (absoluto/relativo) ou apenas o nome do
      arquivo dentro da pasta de voos simulados.
    - Se ``name`` for None, retorna o primeiro CSV da pasta.
    """
    directory = directory or DEFAULT_FLIGHTS_DIR

    if name:
        if os.path.isfile(name):
            return os.path.abspath(name)
        candidate = os.path.join(directory, name)
        if os.path.isfile(candidate):
            return candidate
        raise FileNotFoundError(
            f"voo simulado não encontrado: {name} "
            f"(procurei em {directory})"
        )

    flights = list_simulated_flights(directory)
    if not flights:
        raise FileNotFoundError(
            f"nenhum CSV de voo simulado em {directory}"
        )
    return os.path.join(directory, flights[0])


def load_simulated_flight(
    path: str,
    team_id: str = "#11",
    launch_lat: float = -21.94305,
    launch_lon: float = -48.95409,
) -> List[str]:
    """
    Converte um CSV de telemetria simulada para linhas no formato v2.0 (24 campos).

    O CSV de entrada deve ter as colunas:
        time,altp,temp,p,gx,gy,gz,ax,ay,az,vz,alt,lat,lon

    Os valores do acelerometro (ax/ay/az), giroscopio (gx/gy/gz), velocidade
    vertical (vz), altitude (alt/altp), temperatura (temp), pressao (p) e
    posicao (lat/lon) sao usados exatamente como estao no CSV. lat/lon sao
    tratados como deslocamentos (em graus) relativos ao ponto de lancamento.

    Campos que o CSV nao fornece (umi/hora/data/sat/rssi) recebem placeholders
    fixos, e millis/count/maxAltitude/state/parachute sao derivados de
    time/vz/altp. As linhas sao ordenadas por time para reproduzir o voo.
    """
    lines: List[str] = []
    max_alt = 0.0
    count = 0

    with open(path, newline="", encoding="utf-8") as fh:
        reader = csv.DictReader(fh)
        rows = sorted(reader, key=lambda r: float(r["time"]))
        for row in rows:
            count += 1

            t = float(row["time"])
            altp = float(row["altp"])
            temp = float(row["temp"])
            press = float(row["p"])
            gx = float(row["gx"])
            gy = float(row["gy"])
            gz = float(row["gz"])
            ax = float(row["ax"])
            ay = float(row["ay"])
            az = float(row["az"])
            vz = float(row["vz"])
            alt = float(row["alt"])
            lat = launch_lat + float(row["lat"])
            lon = launch_lon + float(row["lon"])

            max_alt = max(max_alt, altp)

            # Estado de voo derivado da velocidade vertical.
            if vz > 0.5:
                state, parachute = 1, 0
            elif vz < -0.5:
                state, parachute = 5, 1
            else:
                state, parachute = 0, 0

            millis = int(round(t * 1000.0))

            line = (
                f"{team_id},{millis},{count},{altp:.2f},{temp:.2f},0.00,{press:.2f},"
                f"{gx:.4f},{gy:.4f},{gz:.4f},{ax:.4f},{ay:.4f},{az:.4f},"
                f"{vz:.4f},{max_alt:.2f},{state},0,0,{alt:.2f},"
                f"{lat:.6f},{lon:.6f},10,{parachute},-50"
            )
            lines.append(line)

    if not lines:
        raise ValueError(f"CSV de voo vazio ou sem dados: {path}")
    return lines
