#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//=== STRUCTS ==========
struct Pessoa
{
    char nome[50];
    char email[50];
    char senha[20];
    char cpf[12];
    char telefone[10];
};

struct Funcionario
{
    struct Pessoa pessoa;
    char funcao[30];
    float salario;
};

struct Produto
{
    char nome[50];
    int quantidade;
    char origem[50];
    char peso[10];
    char validade[20];
};

//=== UTIL: ler linha e remover newline ==========
void readLine(char *buffer, int size)
{
    if (fgets(buffer, size, stdin) == NULL)
    {
        buffer[0] = '\0';
        return;
    }
    buffer[strcspn(buffer, "\r\n")] = '\0';
}

//=== VALIDAÇÕES ==========
int validarCPF(const char cpf[])
{
    char cpf_limpo[12];
    int i, soma = 0, resto, digito1, digito2;
    int digitos[11];

    if (strlen(cpf) != 11 || !isdigit((unsigned char)cpf[0])) return 0;

    // Verifica se todos os dígitos são iguais (inválido)
    for (i = 1; i < 11; i++) {
        if (cpf[i] != cpf[0]) break;
    }
    if (i == 11) return 0;

    // Extrai dígitos
    for (i = 0; i < 11; i++) {
        digitos[i] = cpf[i] - '0';
    }

    // Calcula 1º dígito verificador
    for (i = 0; i < 9; i++) {
        soma += digitos[i] * (10 - i);
    }
    resto = soma % 11;
    digito1 = (resto < 2) ? 0 : (11 - resto);

    // Calcula 2º dígito verificador
    soma = 0;
    for (i = 0; i < 10; i++) {
        soma += digitos[i] * (11 - i);
    }
    resto = soma % 11;
    digito2 = (resto < 2) ? 0 : (11 - resto);

    // Valida
    return (digitos[9] == digito1 && digitos[10] == digito2);
}

int validarEmail(const char email[])
{
    const char *arroba = strchr(email, '@');
    const char *ponto = strchr(arroba ? arroba + 1 : email, '.');
    return (arroba != NULL && ponto != NULL && ponto > arroba + 1 && strlen(email) > 5);
}

int senhaForte(const char senha[])
{
    int temMaius = 0, temMinus = 0, temNum = 0;
    if (strlen(senha) < 8) return 0;
    for (int i = 0; senha[i]; i++) {
        if (isupper((unsigned char)senha[i])) temMaius = 1;
        else if (islower((unsigned char)senha[i])) temMinus = 1;
        else if (isdigit((unsigned char)senha[i])) temNum = 1;
    }
    return (temMaius && temMinus && temNum);
}

int validarData(const char data[])
{
    int dia, mes, ano;
    if (sscanf(data, "%d/%d/%d", &dia, &mes, &ano) != 3) return 0;
    if (mes < 1 || mes > 12 || dia < 1 || dia > 31 || ano < 1900 || ano > 2100) return 0;
    // Validação básica de dias por mês (pode ser expandida para anos bissextos se necessário)
    int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (dia > diasPorMes[mes - 1]) return 0;
    // Para fevereiro em ano bissexto (simplificado)
    if (mes == 2 && dia == 29 && (ano % 4 != 0 || (ano % 100 == 0 && ano % 400 != 0))) return 0;
    return 1;
}

int validarTelefone(const char tel[])
{
    if (strlen(tel) != 9)
        return 0;
    for (int i = 0; tel[i]; ++i)
        if (!isdigit((unsigned char)tel[i]))
            return 0;
    return 1;
}

//=== INPUT BÁSICO PARA PESSOA ==========
void inputBasic(struct Pessoa *p)
{
    char linha[256];

    printf("Nome completo: ");
    readLine(linha, sizeof(linha));
    strncpy(p->nome, linha, sizeof(p->nome) - 1);
    p->nome[sizeof(p->nome) - 1] = '\0';

    // Validação de e-mail
    while (1) {
        printf("E-mail: ");
        readLine(linha, sizeof(linha));
        if (validarEmail(linha)) {
            strncpy(p->email, linha, sizeof(p->email) - 1);
            p->email[sizeof(p->email) - 1] = '\0';
            break;
        }
        printf("→ E-mail inválido! Deve conter '@' e domínio com '.' (ex: user@exemplo.com).\n");
    }

    // Validação de senha forte
    while (1) {
        printf("Senha (mínimo 8 caracteres, com maiúscula, minúscula e número): ");
        readLine(linha, sizeof(linha));
        if (senhaForte(linha)) {
            strncpy(p->senha, linha, sizeof(p->senha) - 1);
            p->senha[sizeof(p->senha) - 1] = '\0';
            break;
        }
        printf("→ Senha fraca! Deve ter pelo menos 8 caracteres, incluindo uma maiúscula, uma minúscula e um número.\n");
    }

    while (1)
    {
        printf("CPF (11 dígitos, apenas números): ");
        readLine(linha, sizeof(linha));
        if (validarCPF(linha))
        {
            strncpy(p->cpf, linha, sizeof(p->cpf) - 1);
            p->cpf[sizeof(p->cpf) - 1] = '\0';
            break;
        }
        printf("→ CPF inválido! Deve ser um CPF válido com 11 dígitos numéricos.\n");
    }

    while (1)
    {
        printf("Telefone (9 dígitos, apenas números): ");
        readLine(linha, sizeof(linha));
        if (validarTelefone(linha))
        {
            strncpy(p->telefone, linha, sizeof(p->telefone) - 1);
            p->telefone[sizeof(p->telefone) - 1] = '\0';
            break;
        }
        printf("→ Telefone inválido! Deve conter exatamente 9 dígitos numéricos.\n");
    }
}

//=== EXTRAIR JSON ==========
int extract_string(const char *line, const char *key, char *out, size_t size)
{
    char search[32];
    snprintf(search, sizeof(search), "\"%s\":\"", key);
    size_t slen = strlen(search);
    const char *pos = strstr(line, search);
    if (!pos)
        return 0;
    pos += slen;
    const char *end = strchr(pos, '"');
    if (!end)
        return 0;
    size_t len = end - pos;
    if (len >= size)
        len = size - 1;
    strncpy(out, pos, len);
    out[len] = '\0';
    return 1;
}

int extract_int(const char *line, const char *key, int *out)
{
    char search[32];
    snprintf(search, sizeof(search), "\"%s\":", key);
    const char *pos = strstr(line, search);
    if (!pos)
        return 0;
    pos += strlen(search);
    char *next;
    *out = (int)strtol(pos, &next, 10);
    if (next == pos)
        return 0;
    return 1;
}

int extract_float(const char *line, const char *key, float *out)
{
    char search[32];
    snprintf(search, sizeof(search), "\"%s\":", key);
    const char *pos = strstr(line, search);
    if (!pos)
        return 0;
    pos += strlen(search);
    char *next;
    *out = strtof(pos, &next);
    if (next == pos)
        return 0;
    return 1;
}

//=== VERIFICAR LOGIN ==========
int verificarLogin(const char *filename, const char *email, const char *senha)
{
    FILE *f = fopen(filename, "r");
    if (!f)
        return 0;
    char line[1024];
    char e[50], s[20];
    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\r\n")] = '\0';
        if (strlen(line) < 5)
            continue;
        if (extract_string(line, "email", e, sizeof(e)) &&
            strcmp(e, email) == 0 &&
            extract_string(line, "senha", s, sizeof(s)) &&
            strcmp(s, senha) == 0)
        {
            fclose(f);
            return 1;
        }
    }
    fclose(f);
    return 0;
}

//=== CADASTRAR FUNCIONÁRIO ==========
void cadastrarFuncionario()
{
    struct Funcionario f;
    FILE *arquivo;
    char linha[256];
    char continuar = 'N';

    inputBasic(&f.pessoa);

    printf("Função: ");
    readLine(linha, sizeof(linha));
    strncpy(f.funcao, linha, sizeof(f.funcao) - 1);
    f.funcao[sizeof(f.funcao) - 1] = '\0';

    while (1)
    {
        printf("Salário (use ponto decimal se necessário, ex: 2500.00): ");
        readLine(linha, sizeof(linha));
        if (sscanf(linha, "%f", &f.salario) == 1)
            break;
        printf("→ Valor inválido. Digite um número válido.\n");
    }

    char json[1024];
    snprintf(json, sizeof(json),
             "{\"nome\":\"%s\",\"email\":\"%s\",\"senha\":\"%s\",\"cpf\":\"%s\",\"telefone\":\"%s\",\"funcao\":\"%s\",\"salario\":%.2f}",
             f.pessoa.nome, f.pessoa.email, f.pessoa.senha, f.pessoa.cpf, f.pessoa.telefone, f.funcao, f.salario);

    arquivo = fopen("funcionarios.json", "a");
    if (arquivo == NULL)
    {
        printf("\nErro ao abrir o arquivo de funcionários!\n");
        readLine(linha, sizeof(linha));
        return;
    }
    fprintf(arquivo, "%s\n", json);
    fclose(arquivo);

    printf("\nFuncionário cadastrado com sucesso!\n");

    do
    {
        printf("Deseja cadastrar outro funcionário? (S/N): ");
        readLine(linha, sizeof(linha));
        if (linha[0] == '\0')
            continuar = 'N';
        else
            continuar = toupper((unsigned char)linha[0]);
        if (continuar == 'S')
        {
            inputBasic(&f.pessoa);

            printf("Função: ");
            readLine(linha, sizeof(linha));
            strncpy(f.funcao, linha, sizeof(f.funcao) - 1);
            f.funcao[sizeof(f.funcao) - 1] = '\0';

            while (1)
            {
                printf("Salário (use ponto decimal se necessário, ex: 2500.00): ");
                readLine(linha, sizeof(linha));
                if (sscanf(linha, "%f", &f.salario) == 1)
                    break;
                printf("→ Valor inválido. Digite um número válido.\n");
            }

            snprintf(json, sizeof(json),
                     "{\"nome\":\"%s\",\"email\":\"%s\",\"senha\":\"%s\",\"cpf\":\"%s\",\"telefone\":\"%s\",\"funcao\":\"%s\",\"salario\":%.2f}",
                     f.pessoa.nome, f.pessoa.email, f.pessoa.senha, f.pessoa.cpf, f.pessoa.telefone, f.funcao, f.salario);

            arquivo = fopen("funcionarios.json", "a");
            if (arquivo != NULL)
            {
                fprintf(arquivo, "%s\n", json);
                fclose(arquivo);
                printf("\nFuncionário cadastrado com sucesso!\n");
            }
            else
            {
                printf("\nErro ao salvar!\n");
            }
        }
    } while (continuar == 'S');
}

//=== CADASTRAR USUÁRIO ==========
void cadastrarUsuario()
{
    struct Pessoa u;
    FILE *arquivo;
    char linha[256];
    char continuar = 'N';

    inputBasic(&u);

    char json[1024];
    snprintf(json, sizeof(json),
             "{\"nome\":\"%s\",\"email\":\"%s\",\"senha\":\"%s\",\"cpf\":\"%s\",\"telefone\":\"%s\"}",
             u.nome, u.email, u.senha, u.cpf, u.telefone);

    arquivo = fopen("usuarios.json", "a");
    if (arquivo == NULL)
    {
        printf("\nErro ao abrir o arquivo de usuários!\n");
        readLine(linha, sizeof(linha));
        return;
    }
    fprintf(arquivo, "%s\n", json);
    fclose(arquivo);

    printf("\nUsuário cadastrado com sucesso!\n");

    do
    {
        printf("Deseja cadastrar outro usuário? (S/N): ");
        readLine(linha, sizeof(linha));
        if (linha[0] == '\0')
            continuar = 'N';
        else
            continuar = toupper((unsigned char)linha[0]);
        if (continuar == 'S')
        {
            inputBasic(&u);

            snprintf(json, sizeof(json),
                     "{\"nome\":\"%s\",\"email\":\"%s\",\"senha\":\"%s\",\"cpf\":\"%s\",\"telefone\":\"%s\"}",
                     u.nome, u.email, u.senha, u.cpf, u.telefone);

            arquivo = fopen("usuarios.json", "a");
            if (arquivo != NULL)
            {
                fprintf(arquivo, "%s\n", json);
                fclose(arquivo);
                printf("\nUsuário cadastrado com sucesso!\n");
            }
            else
            {
                printf("\nErro ao salvar!\n");
            }
        }
    } while (continuar == 'S');
}

//=== CADASTRAR PRODUTO ==========
void cadastrarProduto()
{
    struct Produto pr;
    FILE *arquivo;
    char linha[256];
    char continuar = 'N';

    printf("==========================================================\n");
    printf("|                 FarmaLife - Cadastro de Produto         |\n");
    printf("==========================================================\n\n");

    printf("Preencha as informações do produto:\n\n");

    printf("Nome do medicamento: ");
    readLine(linha, sizeof(linha));
    strncpy(pr.nome, linha, sizeof(pr.nome) - 1);
    pr.nome[sizeof(pr.nome) - 1] = '\0';

    while (1)
    {
        printf("Quantidade (número positivo): ");
        readLine(linha, sizeof(linha));
        if (sscanf(linha, "%d", &pr.quantidade) == 1 && pr.quantidade > 0)
            break;
        printf("→ Quantidade inválida. Digite um número positivo.\n");
    }

    printf("Origem da doação: ");
    readLine(linha, sizeof(linha));
    strncpy(pr.origem, linha, sizeof(pr.origem) - 1);
    pr.origem[sizeof(pr.origem) - 1] = '\0';

    printf("Peso (ml ou mg, ex: 100mg): ");
    readLine(linha, sizeof(linha));
    strncpy(pr.peso, linha, sizeof(pr.peso) - 1);
    pr.peso[sizeof(pr.peso) - 1] = '\0';

    // Validação de data de validade
    while (1) {
        printf("Validade (dd/mm/yyyy): ");
        readLine(linha, sizeof(linha));
        if (validarData(linha)) {
            strncpy(pr.validade, linha, sizeof(pr.validade) - 1);
            pr.validade[sizeof(pr.validade) - 1] = '\0';
            break;
        }
        printf("→ Data de validade inválida! Use formato dd/mm/yyyy com data válida.\n");
    }

    char json[1024];
    snprintf(json, sizeof(json),
             "{\"nome\":\"%s\",\"quantidade\":%d,\"origem\":\"%s\",\"peso\":\"%s\",\"validade\":\"%s\"}",
             pr.nome, pr.quantidade, pr.origem, pr.peso, pr.validade);

    arquivo = fopen("produtos.json", "a");
    if (arquivo == NULL)
    {
        printf("\nErro ao abrir o arquivo de produtos!\n");
        readLine(linha, sizeof(linha));
        return;
    }
    fprintf(arquivo, "%s\n", json);
    fclose(arquivo);

    printf("\nProduto cadastrado com sucesso!\n");

    do
    {
        printf("Deseja cadastrar outro produto? (S/N): ");
        readLine(linha, sizeof(linha));
        if (linha[0] == '\0')
            continuar = 'N';
        else
            continuar = toupper((unsigned char)linha[0]);
        if (continuar == 'S')
        {
            printf("Nome do medicamento: ");
            readLine(linha, sizeof(linha));
            strncpy(pr.nome, linha, sizeof(pr.nome) - 1);
            pr.nome[sizeof(pr.nome) - 1] = '\0';

            while (1)
            {
                printf("Quantidade (número positivo): ");
                readLine(linha, sizeof(linha));
                if (sscanf(linha, "%d", &pr.quantidade) == 1 && pr.quantidade > 0)
                    break;
                printf("→ Quantidade inválida. Digite um número positivo.\n");
            }

            printf("Origem da doação: ");
            readLine(linha, sizeof(linha));
            strncpy(pr.origem, linha, sizeof(pr.origem) - 1);
            pr.origem[sizeof(pr.origem) - 1] = '\0';

            printf("Peso (ml ou mg, ex: 100mg): ");
            readLine(linha, sizeof(linha));
            strncpy(pr.peso, linha, sizeof(pr.peso) - 1);
            pr.peso[sizeof(pr.peso) - 1] = '\0';

            // Validação de data de validade no loop
            while (1) {
                printf("Validade (dd/mm/yyyy): ");
                readLine(linha, sizeof(linha));
                if (validarData(linha)) {
                    strncpy(pr.validade, linha, sizeof(pr.validade) - 1);
                    pr.validade[sizeof(pr.validade) - 1] = '\0';
                    break;
                }
                printf("→ Data de validade inválida! Use formato dd/mm/yyyy com data válida.\n");
            }

            snprintf(json, sizeof(json),
                     "{\"nome\":\"%s\",\"quantidade\":%d,\"origem\":\"%s\",\"peso\":\"%s\",\"validade\":\"%s\"}",
                     pr.nome, pr.quantidade, pr.origem, pr.peso, pr.validade);

            arquivo = fopen("produtos.json", "a");
            if (arquivo != NULL)
            {
                fprintf(arquivo, "%s\n", json);
                fclose(arquivo);
                printf("\nProduto cadastrado com sucesso!\n");
            }
            else
            {
                printf("\nErro ao salvar!\n");
            }
        }
    } while (continuar == 'S');
}

//=== LISTAGENS ==========
void listarFuncionarios()
{
    FILE *arquivo = fopen("funcionarios.json", "r");
    if (arquivo == NULL)
    {
        printf("\nNenhum funcionário cadastrado!\n\n");
        return;
    }
    printf("\n========== LISTA DE FUNCIONÁRIOS ==========\n\n");
    char linha[1024];
    int count = 0;
    while (fgets(linha, sizeof(linha), arquivo))
    {
        linha[strcspn(linha, "\r\n")] = '\0';
        if (strlen(linha) < 5)
            continue;
        char nome[50], email[50], cpf[12], tel[10], funcao[30];
        float salario;
        if (extract_string(linha, "nome", nome, sizeof(nome)) &&
            extract_string(linha, "email", email, sizeof(email)) &&
            extract_string(linha, "cpf", cpf, sizeof(cpf)) &&
            extract_string(linha, "telefone", tel, sizeof(tel)) &&
            extract_string(linha, "funcao", funcao, sizeof(funcao)) &&
            extract_float(linha, "salario", &salario))
        {
            printf("---------------- FUNCIONÁRIO %d ----------------\n", ++count);
            printf("Nome: %s\n", nome);
            printf("E-mail: %s\n", email);
            printf("CPF: %s\n", cpf);
            printf("Telefone: %s\n", tel);
            printf("Função: %s\n", funcao);
            printf("Salário: %.2f\n", salario);
            printf("---------------------------------------------\n\n");
        }
    }
    if (count == 0)
        printf("Nenhum funcionário encontrado.\n");
    printf("===========================================\n\n");
    fclose(arquivo);
}

void listarUsuarios()
{
    FILE *arquivo = fopen("usuarios.json", "r");
    if (arquivo == NULL)
    {
        printf("\nNenhum usuário cadastrado!\n\n");
        return;
    }
    printf("\n=========== LISTA DE USUÁRIOS ============\n\n");
    char linha[1024];
    int count = 0;
    while (fgets(linha, sizeof(linha), arquivo))
    {
        linha[strcspn(linha, "\r\n")] = '\0';
        if (strlen(linha) < 5)
            continue;
        char nome[50], email[50], cpf[12], tel[10];
        if (extract_string(linha, "nome", nome, sizeof(nome)) &&
            extract_string(linha, "email", email, sizeof(email)) &&
            extract_string(linha, "cpf", cpf, sizeof(cpf)) &&
            extract_string(linha, "telefone", tel, sizeof(tel)))
        {
            printf("----------------- USUÁRIO %d -----------------\n", ++count);
            printf("Nome: %s\n", nome);
            printf("E-mail: %s\n", email);
            printf("CPF: %s\n", cpf);
            printf("Telefone: %s\n", tel);
            printf("-------------------------------------------\n\n");
        }
    }
    if (count == 0)
        printf("Nenhum usuário encontrado.\n");
    printf("===========================================\n\n");
    fclose(arquivo);
}

void listarProdutos()
{
    FILE *arquivo = fopen("produtos.json", "r");
    if (arquivo == NULL)
    {
        printf("\nNenhum produto cadastrado!\n\n");
        return;
    }
    printf("\n=========== LISTA DE PRODUTOS ============\n\n");
    char linha[1024];
    int count = 0;
    while (fgets(linha, sizeof(linha), arquivo))
    {
        linha[strcspn(linha, "\r\n")] = '\0';
        if (strlen(linha) < 5)
            continue;
        char nome[50], origem[50], peso[10], validade[20];
        int quantidade;
        if (extract_string(linha, "nome", nome, sizeof(nome)) &&
            extract_int(linha, "quantidade", &quantidade) &&
            extract_string(linha, "origem", origem, sizeof(origem)) &&
            extract_string(linha, "peso", peso, sizeof(peso)) &&
            extract_string(linha, "validade", validade, sizeof(validade)))
        {
            printf("---------------- PRODUTO %d ----------------\n", ++count);
            printf("Nome do medicamento: %s\n", nome);
            printf("Quantidade: %d\n", quantidade);
            printf("Origem da doação: %s\n", origem);
            printf("Peso: %s\n", peso);
            printf("Validade: %s\n", validade);
            printf("-------------------------------------------\n\n");
        }
    }
    if (count == 0)
        printf("Nenhum produto encontrado.\n");
    printf("===========================================\n\n");
    fclose(arquivo);
}

//=== LOGIN ==========
int loginUsuario(const char *tipo)
{
    char linha[256], email[50], senha[20];

    printf("==========================================================\n");
    printf("|                    FarmaLife - Login                   |\n");
    printf("==========================================================\n\n");

    printf("Acesse com segurança.\n");
    printf("====== Login de %s ======\n\n", tipo);

    printf("E-mail: ");
    readLine(linha, sizeof(linha));
    strncpy(email, linha, sizeof(email) - 1);
    email[sizeof(email) - 1] = '\0';

    printf("Senha: ");
    readLine(linha, sizeof(linha));
    strncpy(senha, linha, sizeof(senha) - 1);
    senha[sizeof(senha) - 1] = '\0';

    if (strcmp(tipo, "Administrador") == 0 &&
        strcmp(email, "admin@farmalife.com") == 0 &&
        strcmp(senha, "admin123") == 0)
        return 1;

    const char *filename = NULL;
    if (strcmp(tipo, "Funcionário") == 0)
        filename = "funcionarios.json";
    else if (strcmp(tipo, "Usuário") == 0)
        filename = "usuarios.json";
    else
        return 0;

    if (verificarLogin(filename, email, senha))
    {
        return (strcmp(tipo, "Funcionário") == 0) ? 2 : 3;
    }

    printf("\nCredenciais inválidas!\n\n");
    return 0;
}

//=== MENUS ==========
void menuAdministrador()
{
    int opcao = 0;
    char linha[32];

    do
    {
        printf("\n====== MENU ADMINISTRADOR ======\n\n");
        printf("1 - Cadastrar Funcionário\n");
        printf("2 - Cadastrar Usuário\n");
        printf("3 - Cadastrar Produto\n");
        printf("4 - Listar Funcionários\n");
        printf("5 - Listar Usuários\n");
        printf("6 - Listar Produtos\n");
        printf("7 - Voltar\n");
        printf("\nEscolha: ");
        readLine(linha, sizeof(linha));
        if (sscanf(linha, "%d", &opcao) != 1)
            opcao = 0;

        switch (opcao)
        {
        case 1:
            cadastrarFuncionario();
            break;
        case 2:
            cadastrarUsuario();
            break;
        case 3:
            cadastrarProduto();
            break;
        case 4:
            listarFuncionarios();
            printf("Pressione ENTER para continuar...");
            readLine(linha, sizeof(linha));
            break;
        case 5:
            listarUsuarios();
            printf("Pressione ENTER para continuar...");
            readLine(linha, sizeof(linha));
            break;
        case 6:
            listarProdutos();
            printf("Pressione ENTER para continuar...");
            readLine(linha, sizeof(linha));
            break;
        case 7:
            break;
        default:
            printf("\nOpção inválida! Pressione ENTER para continuar...");
            readLine(linha, sizeof(linha));
            break;
        }
    } while (opcao != 7);
}

void menuFuncionario()
{
    int opcao = 0;
    char linha[32];

    do
    {
        printf("\n====== MENU FUNCIONÁRIO ======\n\n");
        printf("1 - Cadastrar Produto\n");
        printf("2 - Listar Produtos\n");
        printf("3 - Listar Usuários\n");
        printf("4 - Sair\n");
        printf("\nEscolha: ");
        readLine(linha, sizeof(linha));
        if (sscanf(linha, "%d", &opcao) != 1)
            opcao = 0;

        switch (opcao)
        {
        case 1:
            cadastrarProduto();
            break;
        case 2:
            listarProdutos();
            printf("Pressione ENTER para continuar...");
            readLine(linha, sizeof(linha));
            break;
        case 3:
            listarUsuarios();
            printf("Pressione ENTER para continuar...");
            readLine(linha, sizeof(linha));
            break;
        case 4:
            break;
        default:
            printf("\nOpção inválida! Pressione ENTER para continuar...");
            readLine(linha, sizeof(linha));
            break;
        }
    } while (opcao != 4);
}

void menuUsuario()
{
    int opcao = 0;
    char linha[32];

    do
    {
        printf("\n====== MENU USUÁRIO ======\n\n");
        printf("1 - Listar Usuários\n");
        printf("2 - Sair\n");
        printf("\nEscolha: ");
        readLine(linha, sizeof(linha));
        if (sscanf(linha, "%d", &opcao) != 1)
            opcao = 0;

        switch (opcao)
        {
        case 1:
            listarUsuarios();
            printf("Pressione ENTER para continuar...");
            readLine(linha, sizeof(linha));
            break;
        case 2:
            break;
        default:
            printf("\nOpção inválida! Pressione ENTER para continuar...");
            readLine(linha, sizeof(linha));
            break;
        }
    } while (opcao != 2);
}

//=== MENU PRINCIPAL ==========
int menuPrincipal()
{
    char linha[32];
    int opcao = 0;

    printf("==========================================================\n");
    printf("|                    FarmaLife - Sistema                 |\n");
    printf("==========================================================\n\n");

    printf("Selecione seu tipo de usuário para continuar:\n\n");
    printf("   ==========================================\n");
    printf("   | 1 - Administrador                      |\n");
    printf("   | 2 - Voluntário / Funcionário           |\n");
    printf("   | 3 - Doador / Solicitante               |\n");
    printf("   | 4 - Sair                               |\n");
    printf("   ==========================================\n\n");

    printf("Escolha uma opção: ");
    readLine(linha, sizeof(linha));
    if (sscanf(linha, "%d", &opcao) != 1)
        opcao = 0;
    return opcao;
}

//=== MAIN ==========
int main()
{
    int tipoLogin, escolha;
    char linha[32];

    do
    {
        escolha = menuPrincipal();

        switch (escolha)
        {
        case 1:
            tipoLogin = loginUsuario("Administrador");
            if (tipoLogin == 1)
            {
                menuAdministrador();
            }
            break;
        case 2:
            tipoLogin = loginUsuario("Funcionário");
            if (tipoLogin == 2)
            {
                menuFuncionario();
            }
            break;
        case 3:
            tipoLogin = loginUsuario("Usuário");
            if (tipoLogin == 3)
            {
                menuUsuario();
            }
            break;
        case 4:
            break;
        default:
            printf("\nOpção inválida! Pressione ENTER para continuar...");
            readLine(linha, sizeof(linha));
            break;
        }
    } while (escolha != 4);

    printf("\nSaindo do Sistema...\n\n");
    return 0;
}
