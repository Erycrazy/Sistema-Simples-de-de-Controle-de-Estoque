# Sistema de Controle de Estoque - FarmaLife (Versão Melhorada)

import json
import os
from datetime import datetime

ARQ_PRODUTOS = "produtos.json"


# ============================ FUNÇÕES AUXILIARES ============================

def carregar_produtos():
    """Carrega os produtos a partir de um arquivo JSON (formato linha por objeto)."""
    if not os.path.exists(ARQ_PRODUTOS):
        print(f"⚠ Arquivo '{ARQ_PRODUTOS}' não encontrado!")
        return []

    produtos = []
    try:
        with open(ARQ_PRODUTOS, "r", encoding="utf-8") as f:
            for linha in f:
                linha = linha.strip()
                if linha:
                    try:
                        prod = json.loads(linha)
                        if isinstance(prod, dict):
                            # Adiciona estoque_minimo padrão se não existir
                            if 'estoque_minimo' not in prod:
                                prod['estoque_minimo'] = 10
                            # Inicializa status de quantidade
                            atualizar_status_quantidade(prod)
                            produtos.append(prod)
                    except json.JSONDecodeError:
                        print(f"⚠ Linha inválida ignorada: {linha[:50]}...")
    except Exception as e:
        print(f"❌ Erro ao carregar produtos: {e}")
        return []
    return produtos


def salvar_produtos(produtos):
    """Salva a lista de produtos no arquivo JSON (um objeto por linha)."""
    try:
        with open(ARQ_PRODUTOS, "w", encoding="utf-8") as f:
            for prod in produtos:
                json.dump(prod, f, ensure_ascii=False)
                f.write("\n")
        print("💾 Produtos salvos com sucesso.")
    except Exception as e:
        print(f"❌ Erro ao salvar os produtos: {e}")


def validar_inteiro(mensagem):
    """Lê um número inteiro positivo com tratamento de erro."""
    while True:
        entrada = input(mensagem)
        try:
            valor = int(entrada)
            if valor < 0:
                print("⚠ Digite um número positivo.")
                continue
            return valor
        except ValueError:
            print("❌ Entrada inválida. Digite um número inteiro.")


def atualizar_status_quantidade(med):
    """Atualiza o status de quantidade do medicamento."""
    qtd = med["quantidade"]
    minimo = med["estoque_minimo"]
    if qtd <= 0:
        med["status_quantidade"] = "Zerado"
    elif qtd < minimo:
        med["status_quantidade"] = "Crítico"
    elif qtd == minimo:
        med["status_quantidade"] = "Baixo"
    elif qtd < minimo * 1.5:
        med["status_quantidade"] = "Normal"
    else:
        med["status_quantidade"] = "Alto"


def verificar_validade(validade_str):
    """Verifica a validade e define o status de risco de vencimento."""
    try:
        data_validade = datetime.strptime(validade_str, "%d/%m/%Y")
        hoje = datetime.now()
        delta = data_validade - hoje
        dias_restantes = delta.days
        if dias_restantes < 0:
            return "Vencido"
        elif dias_restantes <= 30:
            return "Crítico"
        elif dias_restantes <= 90:
            return "Alto"
        elif dias_restantes < 365:
            return "Médio"
        else:
            return "Baixo"
    except ValueError:
        return "Data inválida"


# ============================ FUNÇÕES PRINCIPAIS ============================

def mostrar_medicamentos(produtos):
    """Exibe os medicamentos em formato de tabela."""
    if not produtos:
        print("\n⚠ Nenhum medicamento cadastrado.\n")
        return

    print("\n=== Controle de Estoque - FarmaLife ===\n")
    print(f"{'Nome':<25}{'Origem':<20}{'Qtd.':<8}{'Est.Min':<8}{'Status Qtd':<12}"
          f"{'Validade':<12}{'Status Val':<12}")
    print("-" * 115)

    for med in produtos:
        atualizar_status_quantidade(med)
        situacao_val = verificar_validade(med["validade"])
        print(f"{med['nome']:<25}{med['origem']:<20}{med['quantidade']:<8}"
              f"{med['estoque_minimo']:<8}{med['status_quantidade']:<12}"
              f"{med['validade']:<12}{situacao_val:<12}")
    print("-" * 115)


def buscar_medicamento(produtos):
    """Busca e exibe todos os produtos que contenham o nome informado."""
    if not produtos:
        print("⚠ Nenhum produto cadastrado.")
        return

    nome_busca = input("\nDigite o nome do produto para buscar: ").strip()
    if not nome_busca:
        print("❌ Nome não pode estar vazio.")
        return

    matches = [med for med in produtos if nome_busca.lower() in med["nome"].lower()]
    if not matches:
        print("❌ Nenhum produto encontrado com esse nome.")
        return

    print(f"\n🧪 {len(matches)} produto(s) encontrado(s):")
    print(f"{'#':<3}{'Nome':<25}{'Origem':<20}{'Qtd.':<8}{'Validade':<12}{'Status Val':<12}")
    print("-" * 95)

    for i, med in enumerate(matches):
        atualizar_status_quantidade(med)
        status_val = verificar_validade(med["validade"])
        print(f"{i+1:<3}{med['nome']:<25}{med['origem']:<20}{med['quantidade']:<8}"
              f"{med['validade']:<12}{status_val:<12}")
    print("-" * 95)


def selecionar_medicamento(produtos, acao):
    """Seleciona um medicamento com base no nome (suporte a múltiplos)."""
    nome = input(f"\nNome do medicamento para {acao}: ").strip()
    if not nome:
        print("❌ Nome não pode estar vazio.")
        return None

    matches = [med for med in produtos if nome.lower() in med["nome"].lower()]
    if not matches:
        print("❌ Medicamento não encontrado.")
        return None

    if len(matches) == 1:
        return matches[0]

    # Múltiplos encontrados, permite seleção
    print(f"\nMúltiplos produtos encontrados ({len(matches)}):")
    print(f"{'#':<3}{'Nome':<25}{'Origem':<20}{'Qtd. Atual':<12}")
    print("-" * 65)

    for i, med in enumerate(matches):
        print(f"{i+1:<3}{med['nome']:<25}{med['origem']:<20}{med['quantidade']:<12}")

    print("-" * 65)
    try:
        idx = int(input("Selecione o número do produto: ")) - 1
        if 0 <= idx < len(matches):
            return matches[idx]
        else:
            print("❌ Seleção inválida.")
            return None
    except ValueError:
        print("❌ Entrada inválida.")
        return None


def doar_medicamento(produtos):
    """Realiza a doação de um medicamento selecionado."""
    med = selecionar_medicamento(produtos, "doar")
    if med is None:
        return

    quantidade = validar_inteiro("Quantidade a doar: ")
    if med["quantidade"] < quantidade:
        print(f"⚠ Quantidade insuficiente em estoque ({med['quantidade']} disponível).")
        return

    med["quantidade"] -= quantidade
    atualizar_status_quantidade(med)
    salvar_produtos(produtos)
    print(f"✅ Doação realizada: {quantidade} unidade(s) de '{med['nome']}' doadas.")


def receber_medicamento(produtos):
    """Realiza o recebimento de um medicamento selecionado."""
    med = selecionar_medicamento(produtos, "receber")
    if med is None:
        return

    quantidade = validar_inteiro("Quantidade a receber: ")
    med["quantidade"] += quantidade
    atualizar_status_quantidade(med)
    salvar_produtos(produtos)
    print(f"✅ Recebimento realizado: {quantidade} unidade(s) de '{med['nome']}' adicionadas.")


# ============================ MENU PRINCIPAL ============================

def menu():
    produtos = carregar_produtos()
    print("==================== Controle de Estoque - FarmaLife! ====================")

    while True:
        input("\nPressione Enter para atualizar a visualização...")
        mostrar_medicamentos(produtos)
        print("\nAções disponíveis:")
        print("1 - Doar medicamento")
        print("2 - Receber medicamento")
        print("3 - Buscar medicamento por nome")
        print("4 - Sair")

        opcao = input("\nEscolha uma opção: ").strip()

        if opcao == "1":
            doar_medicamento(produtos)
            input("\nPressione Enter para continuar...")
        elif opcao == "2":
            receber_medicamento(produtos)
            input("\nPressione Enter para continuar...")
        elif opcao == "3":
            buscar_medicamento(produtos)
            input("\nPressione Enter para continuar...")
        elif opcao == "4":
            print("\n👋 Encerrando o sistema FarmaLife. Até logo!")
            break
        else:
            print("❌ Opção inválida, tente novamente.")
            input("\nPressione Enter para continuar...")


# ============================ EXECUÇÃO ============================

if __name__ == "__main__":
    menu()