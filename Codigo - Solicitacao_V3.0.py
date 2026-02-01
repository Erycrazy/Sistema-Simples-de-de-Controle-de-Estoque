# FarmaLife - Solicitações de Produtos
import os
import json
from datetime import datetime

ARQ_PENDENTES = "solicitacoes_pendentes.json"
ARQ_HISTORICO = "historico_atendimentos.json"


# === Função para limpar tela ===
def limpar():
    os.system('cls' if os.name == 'nt' else 'clear')


# === Carregar dados de arquivos  ===
def carregar_dados(arquivo):
    if not os.path.exists(arquivo):
        return []
    with open(arquivo, "r", encoding="utf-8") as f:
        try:
            return json.load(f)
        except json.JSONDecodeError:
            return []


# === Salvar dados no arquivo ===
def salvar_dados(arquivo, dados):
    with open(arquivo, "w", encoding="utf-8") as f:
        json.dump(dados, f, indent=4, ensure_ascii=False)

#  Registrar nova solicitação 
def registrar_solicitacao():
    limpar()
    print("=============================================================")
    print("| FarmaLife - Registro de Solicitação de Produtos           |")
    print("=============================================================\n")

    nome = input("Nome do Solicitante: ").strip()
    produto = input("Produto Solicitado: ").strip()
    
    # Validação de quantidade
    while True:
        qtd_str = input("Quantidade Desejada: ").strip()
        # aceita apenas inteiros positivos
        if qtd_str.isdigit():
            qtd = int(qtd_str)
            if qtd > 0:
                break
        print("\n⚠ Quantidade inválida! Digite apenas números inteiros positivos.\n")

    justificativa = input("Justificativa: ").strip()

    if not nome or not produto or qtd <= 0:
        print("\n⚠ Todos os campos obrigatórios devem ser preenchidos!\n")
        input("Pressione ENTER para voltar ao menu...")
        return

    nova = {
        "nome": nome,
        "produto": produto,
        "quantidade": qtd,
        "justificativa": justificativa,
        "data": datetime.now().strftime("%d/%m/%Y %H:%M"),
        "status": "Pendente"
    }

    solicitacoes_pendentes.append(nova)
    salvar_dados(ARQ_PENDENTES, solicitacoes_pendentes)
    print("\n✅ Solicitação registrada com sucesso!\n")
    input("Pressione ENTER para continuar...")


#  solicitações pendentes 
def gerenciar_pendentes():
    limpar()
    print("=============================================================")
    print("| FarmaLife - Solicitações Pendentes                        |")
    print("=============================================================\n")

    if not solicitacoes_pendentes:
        print("Nenhuma solicitação pendente.\n")
        input("Pressione ENTER para voltar...")
        return

    print(f"{'N°':<4} {'Solicitante':<20} {'Produto':<25} {'Qtd':<5} {'Data':<17} {'Status':<10}")
    print("---------------------------------------------------------------------")

    for i, s in enumerate(solicitacoes_pendentes, start=1):
        print(f"{i:<4} {s['nome']:<20} {s['produto']:<25} {s['quantidade']:<5} {s['data']:<17} {s['status']:<10}")

    print("---------------------------------------------------------------------")
    escolha = input("\nDigite o número da solicitação para gerenciar (ou ENTER para sair): ")

    if not escolha:
        return

    try:
        indice = int(escolha) - 1
        sol = solicitacoes_pendentes.pop(indice)
    except (ValueError, IndexError):
        print("\n⚠ Solicitação inválida!")
        input("Pressione ENTER para continuar...")
        return

    print(f"\nGerenciar solicitação de {sol['nome']} - {sol['produto']}")
    acao = input("Digite [A]provar ou [R]eprovar: ").upper()

    if acao == "A":
        sol["status"] = "Aprovado"
        historico_atendimentos.append(sol)
        print("\n✅ Solicitação aprovada com sucesso!\n")
    elif acao == "R":
        sol["status"] = "Reprovado"
        historico_atendimentos.append(sol)
        print("\n❌ Solicitação reprovada!\n")
    else:
        solicitacoes_pendentes.append(sol)
        print("\n⚠ Ação cancelada.\n")

    # Salva as alterações
    salvar_dados(ARQ_PENDENTES, solicitacoes_pendentes)
    salvar_dados(ARQ_HISTORICO, historico_atendimentos)
    input("Pressione ENTER para continuar...")


#  Exibir histórico de atendimentos 
def mostrar_historico():
    limpar()
    print("=============================================================")
    print("| FarmaLife - Histórico de Atendimentos                     |")
    print("=============================================================\n")

    if not historico_atendimentos:
        print("Nenhum atendimento registrado.\n")
    else:
        print(f"{'Solicitante':<20} {'Produto':<25} {'Qtd':<5} {'Data':<17} {'Status Final':<10}")
        print("---------------------------------------------------------------------")

        for s in historico_atendimentos:
            print(f"{s['nome']:<20} {s['produto']:<25} {s['quantidade']:<5} {s['data']:<17} {s['status']:<10}")

        print("---------------------------------------------------------------------\n")

    input("Pressione ENTER para voltar...")


# === Menu principal ===
def tela_solicitacoes():
    while True:
        limpar()
        print("=============================================================")
        print("| FarmaLife - Solicitações de Produtos                      |")
        print("=============================================================\n")
        print("1 - Registrar nova solicitação")
        print("2 - Gerenciar solicitações pendentes")
        print("3 - Ver histórico de atendimentos")
        print("4 - Sair")
        escolha = input("\nEscolha uma opção: ")

        if escolha == '1':
            registrar_solicitacao()
        elif escolha == '2':
            gerenciar_pendentes()
        elif escolha == '3':
            mostrar_historico()
        elif escolha == '4':
            print("\nEncerrando módulo de solicitações...\n")
            break
        else:
            input("\nOpção inválida! Pressione ENTER para tentar novamente...")



if __name__ == "__main__":
    solicitacoes_pendentes = carregar_dados(ARQ_PENDENTES)
    historico_atendimentos = carregar_dados(ARQ_HISTORICO)
    tela_solicitacoes()