import json
from datetime import datetime
import os

ARQ_PRODUTOS = "produtos.json"

def carregar_produtos():
    produtos = []
    if not os.path.exists(ARQ_PRODUTOS):
        print(" O arquivo produtos.json não foi encontrado.")
        return produtos

    with open(ARQ_PRODUTOS, "r", encoding="utf-8") as f:
        for linha in f:
            linha = linha.strip()
            if not linha:
                continue
            try:
                produtos.append(json.loads(linha))
            except json.JSONDecodeError:
                print(f"⚠ Erro ao ler linha inválida: {linha}")
    return produtos


def gerar_relatorio():
    produtos = carregar_produtos()
    if not produtos:
        print("\nNenhum produto encontrado para gerar relatório.\n")
        return

    print("\n=============================================================")
    print("|          RELATÓRIO DE PRODUTOS - FARMALIFE                |")
    print("=============================================================\n")

    total_produtos = len(produtos)
    total_unidades = sum(p["quantidade"] for p in produtos)
    media_unidades = total_unidades / total_produtos if total_produtos else 0

    produto_maior = max(produtos, key=lambda p: p["quantidade"])
    produto_menor = min(produtos, key=lambda p: p["quantidade"])

    # === Vencimentos ===
    hoje = datetime.now()
    vencendo, vencidos = [], []

    for p in produtos:
        try:
            validade = datetime.strptime(p["validade"], "%d/%m/%Y")
            dias_restantes = (validade - hoje).days
            if dias_restantes < 0:
                vencidos.append(p)
            elif dias_restantes <= 60:
                vencendo.append(p)
        except ValueError:
            continue

    # === Ordenar produtos por quantidade ===
    produtos_ordenados = sorted(produtos, key=lambda p: p["quantidade"])
    menores_estoques = produtos_ordenados[:5]  

    print(f"→ Total de produtos cadastrados: {total_produtos}")
    print(f"→ Quantidade total em estoque: {total_unidades}")
    print(f"→ Média de unidades por produto: {media_unidades:.2f}\n")

    print(f"→ Produto com MAIOR quantidade: {produto_maior['nome']} ({produto_maior['quantidade']} un.)")
    print(f"→ Produto com MENOR quantidade: {produto_menor['nome']} ({produto_menor['quantidade']} un.)\n")

    print(f"→ Produtos próximos do vencimento (≤ 60 dias): {len(vencendo)}")
    print(f"→ Produtos vencidos: {len(vencidos)}\n")

    if menores_estoques:
        print("🟡 Produtos com menor quantidade em estoque:")
        for p in menores_estoques:
            print(f"   - {p['nome']} | {p['quantidade']} un. | Validade: {p['validade']}")
        print()

    if vencendo:
        print("🟠 Produtos próximos do vencimento:")
        for p in vencendo:
            print(f"   - {p['nome']} | Validade: {p['validade']} | {p['quantidade']} un.")
        print()

    if vencidos:
        print("🔴 Produtos vencidos:")
        for p in vencidos:
            print(f"   - {p['nome']} | Validade: {p['validade']} | {p['quantidade']} un.")
        print()

    print("=============================================================\n")

    # === Exportar relatório resumido ===
    salvar = input("Deseja exportar o relatório para um arquivo? (S/N): ").upper()
    if salvar == "S":
        with open("relatorio_farmalife.txt", "w", encoding="utf-8") as out:
            out.write("RELATÓRIO DE PRODUTOS - FARMALIFE\n")
            out.write("=================================\n\n")
            out.write(f"Total de produtos: {total_produtos}\n")
            out.write(f"Total em estoque: {total_unidades}\n")
            out.write(f"Média de unidades por produto: {media_unidades:.2f}\n\n")
            out.write(f"Produto com maior quantidade: {produto_maior['nome']} ({produto_maior['quantidade']})\n")
            out.write(f"Produto com menor quantidade: {produto_menor['nome']} ({produto_menor['quantidade']})\n\n")

            out.write("\n--- Produtos com menor estoque ---\n")
            for p in menores_estoques:
                out.write(f"- {p['nome']} | {p['quantidade']} un. | Validade: {p['validade']}\n")

            out.write(f"\nProdutos próximos do vencimento (≤ 60 dias): {len(vencendo)}\n")
            out.write(f"Produtos vencidos: {len(vencidos)}\n\n")
        print("\n✅ Relatório exportado com sucesso (relatorio_farmalife.txt)!\n")


if __name__ == "__main__":
    gerar_relatorio()