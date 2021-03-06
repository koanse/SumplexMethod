#include <stdio.h>
#include <math.h>
#define a_big_number 999999

float **M;	// Расширенная матрица системы (если перенести все в левую часть; в конце - свободные члены)
float *f;	// Коэффициенты целевой функции
int *basis;	// Номера переменных, находящихся в базисе
int m = 2;	// Число уравнений
int n = 5;	// Число неизвестных
// Названия переменных
wchar_t names[200] = L"x<sub>1</sub>x<sub>2</sub>x<sub>3</sub>x<sub>4</sub>x<sub>5</sub>x<sub>6</sub>x<sub>7</sub></sub>x<sub>8</sub>x<sub>9</sub>x<sub>10</sub>";

FILE *out;

void input();											// Ввод матрицы и целевой функции
void print_system();									// Вывод системы уравнений
void print_function();									// Вывод целевой функции
void print_basis();										// Вывод базисных и свободных переменных
void print_basis_decision();							// Вывод базисного решения
void change_basis(int new_var, int old_var, int eq);	// Изменение базиса
int main()
{
	int i, j, k, eq, new_var, old_var;
	float min, max;    
	
	fopen_s(&out, "out.html", "w, ccs=UNICODE");	

	input();

	fwprintf_s(out, L"Перейдем к системе ограничений в виде равенств<br>");
	fwprintf_s(out, L"Введем, при необходимости, дополнительные неотрицательные переменные");

	// Получение допустимого решения
	for(;;)
	{
		print_basis();
		print_function();
		print_system();
		print_basis_decision();
		
		// Выяснение, допустимо ли базисное решение
		for(i = 0; i < m; i++)
			if(M[i][n] > 0) break;
		if(i == m) break;
		
		// Проверка системы ограничений на несовместность
		for(i = 0; i < m; i++)
		{
			for(j = 0; j < n; j++)
			{
				for(k = 0; k < m; k++)
					if(j == basis[k]) break;
				if(k < m) continue;
				
				if(M[i][j] < 0 && M[i][n] > 0) break;
			}
			if(j < n) break;
		}

		if(i == m)
		{
			fwprintf_s(out, L"<br>Допустимое решение не существует");
			fclose(out);
			return 0;
		}

		// Выбор базисной переменной для удаления из базиса
		new_var = j;
		fwprintf_s(out, L"<br>Введем в базис переменную %.13s, вычислим: min{", names + 13 * new_var);
		min = a_big_number;
		for(i = 0; i < m; i++)
		{
			if(i != 0) fwprintf_s(out, L"; ");
            if(M[i][n] > 0 && M[i][new_var] < 0 && (-1) * M[i][n] / M[i][new_var] < min)
			{
				min = (-1) * M[i][n] / M[i][new_var];
				eq = i;
				fwprintf_s(out, L"%.3f", min);
			}
			else fwprintf_s(out, L"∞");
		}
		fwprintf_s(out, L"} = %.3f<br>", min);

		for(i = 0; i < m; i++)
			if(M[eq][basis[i]]) break;
		old_var = basis[i];

		fwprintf_s(out, L"Выберем уравнение %d и выведем из базиса переменную %.13s", eq + 1, names + 13 * old_var);

		change_basis(new_var, old_var, eq);
	}

	fwprintf_s(out, L"Допустимое решение найдено. Найдем оптимальное решение<br>");
	
	// Получение оптимального решения
	for(;;)
	{
		print_basis();
		print_function();
		print_system();
		print_basis_decision();

		// Проверка на достижение оптисального решения
		j = 0;
		k = 0;
		
		// Проверка, достигнут ли максимум									!!!
		for(i = 0; i < n; i++)
		{
			if(f[i] < 0) j++;
			if(f[i] == 0) k++;
		}

		// Проверка, достигнут ли минимум									!!!
		//for(i = 0; i < n; i++)
		//{
        //	if(f[i] > 0) j++;
		//	if(f[i] == 0) k++;
		//}

		if(j == n - m)
		{
			fwprintf_s(out, L"Оптимальное решение найдено<br>");
			fwprintf_s(out, L"F<sub>max</sub> = %.3f", f[n]);
			fclose(out);
			return 0;
		}

		if(j + k == n) fwprintf_s(out, L"<br>Существует несколько оптимальных решений<br>");

		// Выбор переменной, которую нужно ввести в базис
		max = 0;
		new_var = -1;
		for(i = 0; i < n; i++)
		{
			// Для максимума												!!!
			if(f[i] > 0 && f[i] > max)
			{
			max = f[i];
				new_var = i;
			}

		//	// Для минимума													!!!
		//	if(f[i] < 0 && abs(f[i]) > max)		
		//	{
		//		max = abs(f[i]);
		//		new_var = i;
		//	}
		}
		if(new_var == -1) return 0;

		// Выбор базисной переменной для удаления из базиса
		fwprintf_s(out, L"<br>Введем в базис переменную %.13s, вычислим: min {", names + 13 * new_var);
		min = a_big_number;
		for(i = 0; i < m; i++)
		{
			if(i != 0) fwprintf_s(out, L"; ");
            
			if(M[i][new_var] > 0)
			{
				if(abs(M[i][n] / M[i][new_var]) < min)
				{
					eq = i;
					min = abs(M[i][n] / M[i][new_var]);
					fwprintf_s(out, L"%.3f", min);
				}
				else
					fwprintf_s(out, L"%.3f", abs(M[i][n] / M[i][new_var]));
			}
			else fwprintf_s(out, L"∞");
		}
		fwprintf_s(out, L"} = %.3f<br>", min);

		if(min == a_big_number)
		{
			fwprintf_s(out, L"Решение задачи не ограничено");
			fclose(out);
			return 0;
		}

		for(i = 0; i < m; i++)
			if(M[eq][basis[i]]) break;
		old_var = basis[i];

		fwprintf_s(out, L"Выберем уравнение %d и выведем из базиса переменную %.13s", eq + 1, names + 13 * old_var);

		change_basis(new_var, old_var, eq);
	}
	fclose(out);
	return 0;
}
void input()
{
	FILE* in;
	fopen_s(&in, "in.txt", "r, ccs=UNICODE");
	fwscanf(in, L"%d", &n);
	fwscanf(in, L"%d", &m);

	int i, j;
	M = new float *[n + 1];
	for(i = 0; i < n + 1; i++)
		M[i] = new float[m];
	basis = new int[m];
	f = new float[n + 1];

	for(i = 0; i < n + 1; i++)
		fwscanf(in, L"%f", &(f[i]));

	for(i = 0; i < m; i++)
		fwscanf(in, L"%d", &(basis[i]));
	
	for(i = 0; i < m; i++)
		for(j = 0; j < n + 1; j++)
			fwscanf(in, L"%f", &(M[i][j]));
	fclose(in);
	return;
}


void print_system()
{
	int i, j, k;
	bool the_first = true;

	fwprintf_s(out, L"Система ограничений:<br>");
	for(i = 0; i < m; i++)
	{
		the_first = true;
		for(j = 0; j < m; j++)
		{
			if(M[i][basis[j]] == 0) continue;
			if(M[i][basis[j]] > 0 && !the_first) fwprintf_s(out, L"+");
			if(the_first) the_first = false;
			if(M[i][basis[j]] == 1)
			{
				fwprintf_s(out, L"%.13s", names + 13 * basis[j]);
				continue;
			}
			if(M[i][basis[j]] == -1)
			{
				fwprintf_s(out, L"-%.13s", names + 13 * basis[j]);
				continue;
			}
			fwprintf_s(out, L"%.3f%.13s", M[i][basis[j]], names + 13 * basis[j]);
		}
		fwprintf_s(out, L" = ");
		the_first = true;
		for(j = 0; j < n + 1; j++)
		{
			for(k = 0; k < m; k++)
				if(j == basis[k]) break;
			
			if(-M[i][j] == 0 || k < m) continue;
			if(-M[i][j] > 0 && !the_first) fwprintf_s(out, L"+");
			if(the_first) the_first = false;
			if(j == n)
			{
				fwprintf_s(out, L"%.3f", -M[i][j]);
				continue;
			}
			if(-M[i][j] == 1 && j != n)
			{
				fwprintf_s(out, L"%.13s", names + 13 * j);
				continue;
			}
			if(-M[i][j] == -1 && j != n)
			{
				fwprintf_s(out, L"-%.13s", names + 13 * j);
				continue;
			}
			fwprintf_s(out, L"%.3f%.13s", -M[i][j], names + 13 * j);
		}
		fwprintf_s(out, L"<br>");
	}
	return;
}

void print_function()
{
	int i; 
	bool the_first;
	fwprintf_s(out, L"<br>F = ");
	the_first = true;
	for(i = 0; i < n + 1; i++)
	{
		if(f[i] == 0) continue;
		if(f[i] > 0 && !the_first) fwprintf_s(out, L"+");
		if(the_first) the_first = false;
		if(i == n)
		{
			fwprintf_s(out, L"%.3f", f[i]);
			continue;
		}
		if(f[i] == 1)
		{
			fwprintf_s(out, L"%.13s", names + 13 * i);
			continue;
		}
		if(f[i] == -1)
		{
			fwprintf_s(out, L"-%.13s", names + 13 * i);
			continue;
		}		
		fwprintf_s(out, L"%.3f%.13s", f[i], names + 13 * i);
	}
	fwprintf_s(out, L"<br>");
	return;
}

void print_basis()
{
	int i, j;
	fwprintf_s(out, L"<br>Базисные переменные: ");
	for(i = 0; i < m; i++)
		fwprintf_s(out, L"%.13s ", names + 13 * basis[i]);
	
	fwprintf_s(out, L"<br>Небазисные переменные: ");
	for(i = 0; i < n; i++)
	{
		for(j = 0; j < m; j++)
			if(i == basis[j]) break;
		if(j == m) fwprintf_s(out, L"%.13s ", names + 13 * i);
	}
	return;
}

void print_basis_decision()
{
	int i, j, k;
	fwprintf_s(out, L"<br>Базисное решение: ");
	fwprintf_s(out, L"(");
	for(i = 0; i < n; i++)
	{
		for(j = 0; j < m; j++)
			if(i == basis[j]) break;
		if(j < m)
		{
			for(k = 0; k < m; k++)
				if(M[k][i]) break;
			fwprintf_s(out, L"%.3f", -M[k][n]);
		}
		else
			fwprintf_s(out, L"0");
		if(i != n - 1) fwprintf_s(out, L"; ");
	}
	fwprintf_s(out, L")");
	
	for(i = 0; i < m; i++)
		if(M[i][n] > 0) break;
	if(i < m) fwprintf_s(out, L"<br>Базисное решение является недопустимым<br>");
		else fwprintf_s(out, L"<br>Базисное решение является допустимым<br>");
	return;
}


void change_basis(int new_var, int old_var, int eq)
{
	int i, j, k;
	float tmp;
	// Изменение базиса
	for(i = 0; i < m; i++)
		if(basis[i] == old_var)
			basis[i] = new_var;
		
	for(i = 0; i < m; i++)
		for(j = 0; j < m - 1; j++)
			if(basis[j] > basis[j + 1])
			{
				k = basis[j];
				basis[j] = basis[j + 1];
				basis[j + 1] = k;
			}

	// Изменение матрицы системы и коэффициентов целевой функции
	tmp = M[eq][new_var];
	for(i = 0; i < n + 1; i++)
		M[eq][i] /= tmp;

	for(i = 0; i < m; i++)
	{
		if(i == eq) continue;
		tmp = M[i][new_var];
		for(j = 0; j < n + 1; j++)
			M[i][j] -= M[eq][j] * tmp;
	}

	tmp = f[new_var];
	f[new_var] = 0;
	for(i = 0; i < n + 1; i++)
	{
		if(i == new_var) continue;
		f[i] -= M[eq][i] * tmp;
	}
	return;
}