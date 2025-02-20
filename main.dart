import 'package:flutter/material.dart';
import 'package:firebase_core/firebase_core.dart';
import 'package:firebase_database/firebase_database.dart';

void main() async {
  WidgetsFlutterBinding.ensureInitialized();
 

  runApp(const MyApp());
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'App de Sensores (ESP32 + Firebase)',
      theme: ThemeData(
        primarySwatch: Colors.blue,
      ),
      home: const TemperaturePage(),
    );
  }
}

class TemperaturePage extends StatefulWidget {
  const TemperaturePage({super.key});

  @override
  State<TemperaturePage> createState() => _TemperaturePageState();
}

class _TemperaturePageState extends State<TemperaturePage> {
  // Referência ao Realtime Database
  // Se você iniciar o Firebase com a databaseURL manualmente,
  // essa referência usará automaticamente aquela URL.
  final DatabaseReference _dbRef = FirebaseDatabase.instance.ref();

  double? _temperatura; // Armazena o valor lido do Firebase

  @override
  void initState() {
    super.initState();
    // Escuta o nó 'sensores/temperatura' em tempo real
    _dbRef.child('sensores/temperatura').onValue.listen((event) {
      final data = event.snapshot.value;
      setState(() {
        if (data is double) {
          _temperatura = data;
        } else {
          _temperatura = double.tryParse(data.toString());
        }
      });
    });
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Leitura de Temperatura'),
      ),
      body: Center(
        child: _temperatura == null
            ? const CircularProgressIndicator()
            : Text(
                'Temperatura: ${_temperatura!.toStringAsFixed(2)} °C',
                style: const TextStyle(fontSize: 24),
              ),
      ),
    );
  }
}
