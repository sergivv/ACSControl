import sqlite3

def inicializar_base_datos():
    """Crea la estructura inicial de la base de datos"""
    conn = sqlite3.connect('ACSControl.db')
    cursor = conn.cursor()
    
    # Crear tablas (ejecutar solo una vez)
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS dispositivos (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            mac_address VARCHAR(17) UNIQUE NOT NULL,
            ip_address VARCHAR(15),
            nombre VARCHAR(50) NOT NULL,
            ubicacion VARCHAR(100),
            descripcion TEXT
        )
    ''')
    
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS configuracion (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            mac_address VARCHAR(17),
            parametro VARCHAR(50),
            valor FLOAT,
            updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (mac_address) REFERENCES dispositivos(mac_address)
        )
    ''')
    
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS datos_telemetria (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            mac_address VARCHAR(17),
            temperatura FLOAT,
            estado VARCHAR(20),
            timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (mac_address) REFERENCES dispositivos(mac_address)
        )
    ''')
    
    cursor.execute('''
        CREATE TABLE IF NOT EXISTS eventos (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            mac_address VARCHAR(17),
            tipo VARCHAR(50),
            mensaje TEXT,
            datos TEXT,
            timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (mac_address) REFERENCES dispositivos(mac_address)
        )
    ''')
    
    conn.commit()
    conn.close()
    print("Base de datos inicializada correctamente")

# Ejecutar la función para inicializar la base de datos
inicializar_base_datos()