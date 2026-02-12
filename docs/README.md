# Documentação do Recovery WebUI

Bem-vindo à documentação completa do Recovery WebUI! Esta pasta contém guias detalhados sobre instalação, arquitetura, API e desenvolvimento.

## 📚 Índice de Documentação

### [Guia de Instalação](installation.md)

Instruções detalhadas de instalação para diferentes sistemas operacionais.

**Conteúdo:**

- Requisitos do sistema
- Instalação no Linux, Windows e macOS
- Configuração de permissões
- Instalação de drivers
- Solução de problemas comuns
- Configuração de ambiente virtual

**Recomendado para:** Usuários iniciantes e administradores de sistema

---

### [Arquitetura do Sistema](architecture.md)

Detalhes técnicos sobre a arquitetura e design do sistema.

**Conteúdo:**

- Visão geral da arquitetura
- Componentes backend e frontend
- Fluxo de dados
- Comunicação serial
- Sistema de logs
- Decisões de design
- Diagramas e considerações de performance

**Recomendado para:** Desenvolvedores que querem entender o funcionamento interno

---

### [Documentação da API](api.md)

Referência completa de endpoints HTTP e eventos WebSocket.

**Conteúdo:**

- Endpoints HTTP (/, /satellite)
- Eventos WebSocket (updateRocket, updateSat)
- Formato de dados serial e logs
- Exemplos de uso
- Códigos de erro
- Cliente Python e JavaScript

**Recomendado para:** Desenvolvedores construindo integrações

---

### [Guia de Desenvolvimento](development.md)

Informações para desenvolvedores contribuindo com o projeto.

**Conteúdo:**

- Configuração do ambiente de desenvolvimento
- Estrutura do código
- Padrões de código
- Testes
- Debugging
- Adicionando funcionalidades
- Workflow de contribuição

**Recomendado para:** Contribuidores e desenvolvedores avançados

---

### [Protocolo de Comunicação](protocol.md)

Especificação detalhada do protocolo LoRa e formato de dados.

**Conteúdo:**

- Especificações técnicas LoRa
- Formato dos pacotes CSV
- Campos de dados detalhados
- Identificadores de dispositivos
- Exemplos de pacotes reais
- Tratamento de erros
- Boas práticas de implementação

**Recomendado para:** Desenvolvedores de firmware e integradores

---

### [FAQ - Perguntas Frequentes](faq.md)

Respostas para dúvidas comuns.

**Conteúdo:**

- Perguntas gerais sobre o projeto
- Problemas de instalação
- Dúvidas de uso
- Questões de hardware e conectividade
- Troubleshooting
- Suporte

**Recomendado para:** Todos os usuários

---

## 🚀 Início Rápido

Se você está começando agora:

1. **Instalar o sistema**: Comece pelo [Guia de Instalação](installation.md)
2. **Entender o básico**: Leia o [README principal](../README.md)
3. **Integrar com sua aplicação**: Consulte a [Documentação da API](api.md)
4. **Contribuir**: Siga o [Guia de Desenvolvimento](development.md)

## 🔍 Procurando por algo específico?

### Instalação e Configuração

- [Como instalar no Linux](installation.md#instalação-no-linux)
- [Como instalar no Windows](installation.md#instalação-no-windows)
- [Permissões da porta serial](installation.md#configuração-de-permissões)
- [Drivers USB](installation.md#drivers-serial-no-windows)

### Uso

- [Como conectar dispositivos](../README.md#uso)
- [Monitorar dados em tempo real](../README.md#funcionalidades-principais)
- [Formato dos dados](api.md#formato-de-dados)

### Desenvolvimento

- [Estrutura do código](development.md#estrutura-do-código)
- [Adicionar novas funcionalidades](development.md#adicionando-funcionalidades)
- [Debugging](development.md#debugging)
- [Contribuir com o projeto](development.md#contribuindo)

### Troubleshooting

- [Problemas comuns de instalação](installation.md#problemas-comuns)
- [Códigos de erro](api.md#códigos-de-erro)
- [Debugging em desenvolvimento](development.md#debugging)
- [FAQ completo](faq.md)

### Hardware e Conectividade

- [Especificações LoRa](protocol.md#especificações-técnicas)
- [Módulos recomendados](faq.md#qual-módulo-lora-devo-usar)
- [Alcance esperado](faq.md#qual-o-alcance-esperado)
- [Antenas](faq.md#que-tipo-de-antena-devo-usar)

## 📊 Diagramas

Diagramas detalhados estão disponíveis em:

- [Arquitetura de Alto Nível](architecture.md#arquitetura-de-alto-nível)
- [Fluxo de Dados](architecture.md#fluxo-de-dados)
- [Estrutura de Componentes](architecture.md#componentes-backend)

## 🤝 Contribuindo com a Documentação

Encontrou um erro ou quer melhorar a documentação?

1. A documentação está em formato Markdown
2. Siga o mesmo estilo dos documentos existentes
3. Adicione exemplos práticos quando possível
4. Inclua screenshots se necessário (coloque em `docs/images/`)

## 📝 Changelog da Documentação

### Versão 1.0 (Janeiro 2025)

- ✅ Documentação inicial completa
- ✅ Guia de instalação para Linux, Windows e macOS
- ✅ Arquitetura do sistema detalhada
- ✅ Documentação completa da API
- ✅ Guia de desenvolvimento

## 🔗 Links Úteis

### Documentação Externa

- [Flask Documentation](https://flask.palletsprojects.com/)
- [Flask-SocketIO](https://flask-socketio.readthedocs.io/)
- [PySerial](https://pyserial.readthedocs.io/)
- [Leaflet.js](https://leafletjs.com/reference.html)
- [Socket.IO](https://socket.io/docs/v4/)

### Recursos

- [Python.org](https://www.python.org/)
- [MDN Web Docs](https://developer.mozilla.org/)
- [PEP 8 Style Guide](https://pep8.org/)

## 📧 Suporte

- **Issues**: [GitHub Issues](../../issues)
- **Discussões**: [GitHub Discussions](../../discussions)
- **Email**: Abra uma issue para contato

---

**Última atualização:** Janeiro 2025

[← Voltar ao README Principal](../README.md)
